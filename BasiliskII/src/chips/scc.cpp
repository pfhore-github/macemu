#include "scc.h"
#include "scc_impl.h"
#include <boost/crc.hpp>
#include <mutex>
SCC_impl::SCC_impl(SCC *parent, bool is_modem)
    : parent(parent), is_modem(is_modem) {
    send_char = {};
    external_pending = false;
    trans_pending = false;
    recv_avail_pending = false;
    recv_special_pending = false;
}
void SCC::reset() {
    IUS = SCC_INT::NONE;
    modem->reset();
    printer->reset();
}

SCC::SCC() {
    modem = std::make_unique<Z8530>(this, true);
    printer = std::make_unique<Z8530>(this, false);
    vec_f = 0;
    reset();
}

void SCC::write_MIC(uint8_t v) {
    switch(v >> 6 & 3) {
    case 0:
        break; // do nothing
    case 1:
        printer->ch_reset();
        break;
    case 2:
        modem->ch_reset();
        break;
    case 3:
        reset();
        break;
    }
    VIS = v & 1;
    NV = v >> 1 & 1;
    DLC = v >> 2 & 1;
    MIE = v >> 3 & 1;
    status_H = v >> 4 & 1;
    INTACK = v >> 5 & 1;
}
void SCC_impl::reset() {
    ch_reset();
    write_reg_impl(11, 8);
    write_reg_impl(14, 0x30);
}

void SCC_impl::ch_reset() {
    std::lock_guard lx(queue_guard);
    reg_ptr = 0;
    top_error = 0;
    latch_enable = false;
    current_rr0 = uint8_t(RR0::TX_OVERRUN_EOM) | uint8_t(RR0::TX_BUFFER_EMPTY);
    first_letter = true;
    sync_sending = false;
    read_buffers.clear();
    write_reg_impl(0, 0);
    write_reg_impl(1, 0);
    write_reg_impl(3, 0);
    write_reg_impl(4, 4);
    write_reg_impl(5, 0);
    write_reg_impl(7, 0x20);
    write_reg_impl(9, 0);
    write_reg_impl(10, 0);
    write_reg_impl(14, 0x20);
    write_reg_impl(15, 0xF8);
    residue_codes = 3;
    end_of_frame = false;
}
// ASYNC: [data] [SSSxxxxP](S=stop;P=parity)
void SCC_impl::recieve_async(std::deque<uint8_t> &src) {
    std::lock_guard lx(queue_guard);
    if(src.empty()) {
        // break
        read_buffers.push_back(0x0000);
        int_external(RR0::BREAK_ABORT);
        return;
    }
    current_rr0 &= uint8_t(RR0::BREAK_ABORT) ^ 0xff;
    if(src.size() != 2 || (src[1] & 0xE0) != async_mode) {
        // framing error
        read_buffers.push_back(0x4000);
        return;
    }
    uint16_t v = src[0];
    if(recv_sync_char_load_inhibit && sync_size != SYNC_SIZE::SIZE_12 &&
       v == sync_char[0]) {
        return;
    }
    if(parity_enable) {
        bool parity = (!parity_even) ^ __builtin_parity(v);
        if(parity != (src[1] & 1)) {
            v |= 0x1000;
        }
        if(recv_size != 8) {
            v |= (src[1] & 1) << recv_size;
        }
    }
    switch(recv_size + parity_enable) {
    case 5:
        v |= 0xE0;
        break;
    case 6:
        v |= 0xC0;
        break;
    case 7:
        v |= 0x80;
        break;
    }
    read_buffers.push_back(v);
    current_rr0 |= uint8_t(RR0::RX_CHAR_AVAILABLE);
    return;
}
// [sync1] [sync2]? [data]*
void SCC_impl::recieve_sync(std::deque<uint8_t> &src) {
    if(src.empty()) {
        return;
    }
    current_rr0 &= uint8_t(RR0::BREAK_ABORT) ^ 0xff;
    uint16_t sync_v = 0;
    uint16_t recv_sync_v = 0;
    switch(sync_size) {
    case SYNC_SIZE::SIZE_6:
        sync_v = sync_char[1] & 0x3f;
        recv_sync_v = src[0] & 0x3f;
        break;
    case SYNC_SIZE::SIZE_8:
        sync_v = sync_char[1];
        recv_sync_v = src[0];
        break;
    case SYNC_SIZE::SIZE_12:
        sync_v = (sync_char[1] << 4) | (sync_char[0] >> 4);
        recv_sync_v = src[0] << 8 | src[1];
        break;
    case SYNC_SIZE::SIZE_16:
        sync_v = sync_char[1] << 8 | sync_char[0];
        recv_sync_v = src[0] << 8 | src[1];
        break;
    case SYNC_SIZE::EXTERNAL:
        /* */
        break;
    }
    if(recv_sync_char_load_inhibit && sync_size != SYNC_SIZE::SIZE_12 &&
       src[0] == sync_char[0]) {
        src.pop_front();
    }

    if(recv_hunt_mode) {
        if(recv_sync_v != sync_v) {
            // skip data
            return;
        } else {
            recv_hunt_mode = false;
            recv_crc16.reset(crc_init ? 0xffff : 0);
            recv_crc_ccitt.reset(crc_init ? 0xffff : 0);
            int_external(RR0::SYNC_HUNT);
        }
    }
    for(uint8_t v : src) {
        uint16_t vx = v & ((1 << recv_size) - 1);
        read_buffers.push_back(vx);
    }
    current_rr0 |= uint8_t(RR0::RX_CHAR_AVAILABLE);
}
// [ $7E ] [ addr ] [ ctl ] [data]* [crc16]
void SCC_impl::recieve_sdlc(std::deque<uint8_t> &src) {
    if(src.empty() || src[0] != 0x7E) {
        return;
    }
    src.pop_front();
    uint8_t addr = src[0];
    src.pop_front();
    if(addr != 0xff) {
        if(recv_sync_char_load_inhibit) {
            if((addr & 0xf0) != (sync_char[0] & 0xf0)) {
                return;
            }
        } else {
            if(addr != sync_char[0]) {
                return;
            }
        }
    }
    uint16_t vx = addr & ((1 << recv_size) - 1);
    read_buffers.push_back(vx);
    for(uint8_t v : src) {
        uint16_t vx = v & ((1 << recv_size) - 1);
        read_buffers.push_back(vx);
    }
    current_rr0 |= uint8_t(RR0::RX_CHAR_AVAILABLE);
}

// Data Recieve
void SCC_impl::recieve_xd(const std::deque<uint8_t> &in) {
    if(!recv_enable) {
        return;
    }
    // data arrive
    if(!w_req_is_wait_mode && w_req_is_recv_mode) {
        //		machine->via1->scc_wr_req = false;
    }
    interrupt(SCC_INT::RECV_AVAIL);
    current_rr0 &= uint8_t(RR0::RX_CHAR_AVAILABLE) ^ 0xff;
    std::deque<uint8_t> src = in;
    if(async_mode) {
        recieve_async(src);
    } else if(!sdlc_mode) {
        recieve_sync(src);
    } else {
        recieve_sdlc(src);
    }
}
void SCC_impl::hsk_i(bool v) {
    if(v && !(current_rr0 & uint8_t(RR0::CTS))) {
        int_external(RR0::CTS);
    } else if(!v && (current_rr0 & uint8_t(RR0::CTS))) {
        current_rr0 &= uint8_t(RR0::CTS) ^ 0xff;
    }
    if(auto_enable && v) {
        send_enable = true;
        write_data();
    }
}

void SCC_impl::gp_i(bool b) {
    if(b && !(current_rr0 & uint8_t(RR0::DCD))) {
        int_external(RR0::DCD);
    } else if(!b && (current_rr0 & uint8_t(RR0::DCD))) {
        current_rr0 &= uint8_t(RR0::DCD) ^ 0xff;
    }
    if(auto_enable && b) {
        recv_enable = true;
    }
}
uint8_t SCC_impl::read_data() {
    std::lock_guard lx(queue_guard);
    if(!read_buffers.empty()) {
        uint16_t v = read_buffers.front();
        read_buffers.pop_front();
        recv_avail_pending = false;
        recv_data = v & 0xff;
        if(enable_crc) {
            if(crc_is_16) {
                recv_crc16.process_byte(recv_data);
                if(recv_crc16.checksum() != 0) {
                    v |= 0x4000;
                }
            } else {
                recv_crc_ccitt.process_byte(recv_data);
                if(recv_crc_ccitt.checksum() != 0) {
                    v |= 0x4000;
                }
            }
        }
        top_error = ((v >> 8) & 0x40) | ((top_error | (v >> 8)) & 0x30);
        if((parity_error_is_special && (v & 0x1000)) || // parity error
           (async_mode && (v & 0x4000))                 // framing error
        ) {
            first_letter = true;
            interrupt(SCC_INT::RECV_SPECIAL);
        }
    } else {
        if(w_req_is_wait_mode && w_req_is_recv_mode) {
            //			machine->via1->scc_wr_req = false;
        }
    }
    if(read_buffers.empty()) {
        // transmission done; calc CRC
        if(0 != crc_is_16 ? recv_crc16.checksum() : recv_crc_ccitt.checksum()) {
            // CRC ERROR
            top_error |= 0x4000;
            interrupt(SCC_INT::RECV_SPECIAL);
        }
    }
    return recv_data;
}
uint8_t SCC_impl::decode_byte(uint8_t w) {
    switch(send_size) {
    case 8:
        return w;
    case 7:
        return w & 0x7f;
    case 6:
        return w & 0x3f;
    case 5:
        if((w & 0xf0) == 0xf0) {
            return w & 1;
        } else if((w & 0xe0) == 0xe0) {
            return w & 3;
        } else if((w & 0xc0) == 0xc0) {
            return w & 7;
        } else if((w & 0x80) == 0x80) {
            return w & 0xf;
        } else {
            return w & 0x1f;
        }
    }
    return 0;
}

// transmit data
void SCC_impl::write_data() {
    if(!send_enable || !send_char) {
        return;
    }
    uint8_t val = decode_byte(*send_char);
    if(async_mode) {
        if(!rts) {
            return;
        }
        //		device->transmit_xd( val );
        bool parity = __builtin_parity(val) ^ !parity_even;
        //        device->transmit_xd(async_mode | (parity_enable ? parity :
        //        0));
    } else if(!sdlc_mode) {
        if(!std::exchange(sync_sending, true)) {
            switch(sync_size) {
            case SYNC_SIZE::SIZE_6: /* device->transmit_xd( sync_char[0] & 0x3f
                                       ); */
                break;
            case SYNC_SIZE::SIZE_8:
//                device->transmit_xd(sync_char[0]);
                break;
            case SYNC_SIZE::SIZE_12: /* no 12bit sync char for send*/
            case SYNC_SIZE::SIZE_16:
//                device->transmit_xd(sync_char[0]);
//                device->transmit_xd(sync_char[1]);
                break;
            case SYNC_SIZE::EXTERNAL:
                /* TODO: external sync mode */
                break;
            }
        }
//        device->transmit_xd(val);
        if(enable_crc) {
            if(crc_is_16) {
                send_crc16.process_byte(val);
            } else {
                send_crc_ccitt.process_byte(val);
            }
        }
    } else {
        if(!std::exchange(sync_sending, true)) {
            // SDLC
//            device->transmit_xd(0x7E);
        }
//        device->transmit_xd(val);
        if(enable_crc) {
            send_crc_ccitt.process_byte(val);
        }
    }
    all_sent.store(true);
    send_char = {};
    current_rr0 |= uint8_t(RR0::TX_BUFFER_EMPTY);
    if(w_req_is_wait_mode && !w_req_is_recv_mode) {
//        machine->via1->scc_wr_req = false;
    }
    interrupt(SCC_INT::TRANS);
}
uint8_t SCC::read(int addr) {
    switch(addr) {
    case SCC_REG::B_CMD:
        return printer->read_reg();
    case SCC_REG::A_CMD:
        return modem->read_reg();
    case SCC_REG::B_DATA:
        return printer->read_data();
    case SCC_REG::A_DATA:
        return modem->read_data();
    default:
        return 0;
    }
}
void SCC::write(int addr, uint8_t v) {
    switch(addr) {
    case SCC_REG::B_CMD:
        printer->write_reg(v);
        break;
    case SCC_REG::A_CMD:
        modem->write_reg(v);
        break;
    case SCC_REG::B_DATA:
        printer->write_wr8(v);
        break;
    case SCC_REG::A_DATA:
        modem->write_wr8(v);
        break;
    }
}
uint8_t SCC_impl::read_reg() {
    int reg = reg_ptr;
    reg_ptr = 0;
    return read_reg_impl(reg);
}

void SCC_impl::write_reg(uint8_t v) {
    write_reg_impl(std::exchange(reg_ptr, 0), v);
}
uint8_t SCC_impl::read_reg_impl(int reg) {
    uint8_t v = 0;
    switch(reg) {
    case 0: // Buffer Status
    case 4:
        if(latch_enable) {
            v = (external_status_int_enable & latched_rr0) |
                ((external_status_int_enable ^ 0xff) & current_rr0);
        } else {
            v = current_rr0;
        }
        break;
    case 1:
    case 5:
        v = end_of_frame << 7 | top_error | // get from error FIFO
            residue_codes << 1 | all_sent;
        break;
    case 2: // Interrupt vector; taken from parent
        if(is_modem) {
            v = parent->vec_f.load();
        } else {
            if(parent->modem->recv_avail_pending) {
                v = parent->status_H ? 0x30 : 0xC;
            } else if(parent->modem->recv_special_pending) {
                v = parent->status_H ? 0x70 : 0xE;
            } else if(parent->modem->trans_pending) {
                v = parent->status_H ? 0x10 : 0x8;
            } else if(parent->modem->external_pending) {
                v = parent->status_H ? 0x50 : 0xA;
            } else if(recv_avail_pending) {
                v = parent->status_H ? 0x20 : 0x4;
            } else if(recv_special_pending) {
                v = parent->status_H ? 0x60 : 0x6;
            } else if(trans_pending) {
                v = 0;
            } else if(external_pending) {
                v = parent->status_H ? 0x40 : 0x2;
            }
        }
        break;
    case 3: // Interrupt pending vector; only modem port
        if(is_modem) {
            v = parent->get_int_vec();
        } else {
            v = 0;
        }
        break;
    case 8: // Recieve Data Register
        v = read_data();
        break;
    case 9:
    case 13:
        return rr[13];
    default:
        return rr[reg & 15];
    }
    return v;
}

void SCC_impl::write_reg0(uint8_t v) {
    reg_ptr = v & 7;
    switch((v >> 3) & 7) {
    case 0:
        break;
    case 1:
        reg_ptr |= 8;
        break;
    case 2:
        // RESET Ext/Status Interrupts
        latch_enable = false;
        external_pending = false;
        break;
    case 3:
        // SEND Abort SDLC
//        device->transmit_xd(0xff);
//        device->transmit_xd(0xff);
        int_external(RR0::TX_OVERRUN_EOM);
        break;
    case 4:
        // Enable Int on Next Rx Character
        first_letter = true;
        break;
    case 5: {
        // Reset Tx Int Pending
        trans_pending = false;
        if(!async_mode && !(current_rr0 & uint8_t(RR0::TX_OVERRUN_EOM))) {
            // transmit CRC
            sync_done();
            current_rr0 |= uint8_t(RR0::TX_OVERRUN_EOM);
        }
        break;
    case 6:
        // ERROR reset
        top_error = 0;
        recv_special_pending = false;
        break;
    }
    case 7:
        // RESET Highest IUS
        parent->IUS = SCC_INT::NONE;
        break;
    }
    switch((v >> 6) & 3) {
    case 0:
        break;
    case 1:
        // RESET Rx CRC
        recv_crc16.reset(crc_init ? 0xffff : 0);
        recv_crc_ccitt.reset(crc_init ? 0xffff : 0);
        break;
    case 2:
        // Reset Tx CRC
        send_crc16.reset(crc_init ? 0xffff : 0);
        send_crc_ccitt.reset(crc_init ? 0xffff : 0);
        break;
    case 3:
        // Reset Tx Underrun
        current_rr0 &= uint8_t(RR0::TX_OVERRUN_EOM) ^ 0xff;
        break;
    }
}

void SCC_impl::int_external(RR0 v) {
    current_rr0 |= uint8_t(v);
    if(v == RR0::BREAK_ABORT &&
       (external_status_int_enable & uint8_t(RR0::BREAK_ABORT))) {
        latched_rr0 |= current_rr0.load();
    }
    if(!latch_enable && (external_status_int_enable & uint8_t(v))) {
        interrupt(SCC_INT::EXTERNAL);
    }
}

void SCC_impl::write_reg_impl(int n, uint8_t v) {
    switch(n) {
    case 0:
        write_reg0(v);
        return;
    case 1:
        w_req_is_wait_mode = v >> 7 & 1;
        w_req_is_recv_mode = v >> 6 & 1;
        recv_interrupt_mode = v >> 3 & 3;
        parity_error_is_special = v >> 2 & 1;
        transmit_empty_interrupt = v >> 1 & 1;
        external_interrupt_enable = v & 1;
        break;
    case 2: {
        parent->vec_f = v;
        parent->modem->recv_avail_pending = v >> 5 & 1;
        parent->modem->recv_special_pending = v >> 5 & 1;
        parent->modem->trans_pending = v >> 4 & 1;
        parent->modem->external_pending = v >> 3 & 1;
        parent->printer->recv_avail_pending = v >> 2 & 1;
        parent->printer->recv_special_pending = v >> 2 & 1;
        parent->printer->trans_pending = v >> 1 & 1;
        parent->printer->external_pending = v & 1;
        break;
    }
    case 3:
        switch((v >> 6) & 3) {
        case 0:
            recv_size = 5;
            break;
        case 1:
            recv_size = 7;
            break;
        case 2:
            recv_size = 6;
            break;
        case 3:
            recv_size = 8;
            break;
        }
        auto_enable = v >> 5 & 1;
        if(auto_enable) {
            rts = true;
        }
        if(v >> 4 & 1) {
            recv_hunt_mode = true;
            int_external(RR0::SYNC_HUNT);
        } else {
            recv_hunt_mode = false;
        }
        recv_crc_enable = v >> 3 & 1;
        recv_addr_search_mode = v >> 2 & 1;
        recv_sync_char_load_inhibit = v >> 1 & 1;
        recv_enable = v & 1;

        break;
    case 4: {
        parity_enable = v & 1;
        parity_even = v >> 1 & 1;
        switch((v >> 2) & 3) {
        case 0:
            async_mode = 0;
            break;
        case 1:
            async_mode = ASYNC_STOP_1;
            break;
        case 2:
            async_mode = ASYNC_STOP_1_HALF;
            break;
        case 3:
            async_mode = ASYNC_STOP_2;
            break;
        }
        if(async_mode) {
            current_rr0 |= uint8_t(RR0::TX_OVERRUN_EOM);
        }
        sdlc_mode = false;
        switch((v >> 4) & 3) {
        case 0:
            sync_size = sync_6bit ? SYNC_SIZE::SIZE_6 : SYNC_SIZE::SIZE_8;
            break;
        case 1:
            sync_size = sync_6bit ? SYNC_SIZE::SIZE_12 : SYNC_SIZE::SIZE_16;
            break;
        case 2:
            sdlc_mode = true;
            break;
        case 3:
            sync_size = SYNC_SIZE::EXTERNAL;
            break;
            return;
        }
        break;
    }
    case 5:
        if(dtr_enable && v & 0x80) {
            device->hsk_o();
        }
        switch((v >> 5) & 3) {
        case 0:
            send_size = 5;
            break;
        case 1:
            send_size = 7;
            break;
        case 2:
            send_size = 6;
            break;
        case 3:
            send_size = 8;
            break;
        }
        if(v & 1 << 4) {
            device->transmit_xd(0x00);
            device->transmit_xd(0x00);
            return;
        }
        send_enable = v >> 3 & 1;
        crc_is_16 = v >> 2 & 1;
        if(!(v & 2)) {
            if(async_mode) {
                if(!auto_enable) {
                    rts = false;
                }
            }
        } else {
            rts = true;
        }
        write_data();
        enable_crc = v & 1;
        return;
    case 6:
        sync_char[0] = v;
        break;
    case 7:
        sync_char[1] = v;
        break;
    case 8: // transmit buffer
        write_wr8(v);
        break;
    case 9:
        parent->write_MIC(v);
        return;
    case 10:
        sync_6bit = v & 1;
        switch(sync_size) {
        case SYNC_SIZE::SIZE_6:
        case SYNC_SIZE::SIZE_8:
            sync_size = sync_6bit ? SYNC_SIZE::SIZE_6 : SYNC_SIZE::SIZE_8;
            break;
        case SYNC_SIZE::SIZE_12:
        case SYNC_SIZE::SIZE_16:
            sync_size = sync_6bit ? SYNC_SIZE::SIZE_12 : SYNC_SIZE::SIZE_16;
            break;
        case SYNC_SIZE::EXTERNAL:
            break;
        }
        crc_init = v >> 7 & 1;
        break;
    case 15:
        external_status_int_enable = v & 0xFA; // BIT0, 2 is reserved
    }
}

void SerialDevice::recieve_xd(const std::deque<uint8_t> &v) {
    connected_to.lock()->recieve_xd(v);
}
void SerialDevice::hsk_i(bool v) { connected_to.lock()->hsk_i(v); }

void SerialDevice::gp_i(bool v) { connected_to.lock()->gp_i(v); }

void SCC_impl::write_wr8(uint8_t v) {
    send_char = v;
    current_rr0 &= uint8_t(RR0::TX_BUFFER_EMPTY) ^ 0xff;
    trans_pending = false;
    write_data();
}
void SCC_impl::interrupt(SCC_INT i) {
    switch(i) {
    case SCC_INT::TRANS:
        if(transmit_empty_interrupt) {
            parent->interrupt(is_modem ? SCC_INT::TRANS_A : SCC_INT::TRANS_B);
        }
        break;
    case SCC_INT::RECV_AVAIL:
        if((recv_interrupt_mode == 1 && std::exchange(first_letter, false)) ||
           recv_interrupt_mode == 2) {
            parent->interrupt(is_modem ? SCC_INT::RECV_AVAIL_A
                                       : SCC_INT::RECV_AVAIL_B);
        }
        break;
    case SCC_INT::RECV_SPECIAL:
        if(recv_interrupt_mode) {
            first_letter = true;
            parent->interrupt(is_modem ? SCC_INT::RECV_SPECIAL_A
                                       : SCC_INT::RECV_SPECIAL_B);
        }
        break;
    case SCC_INT::EXTERNAL:
        if(external_interrupt_enable) {
            parent->interrupt(is_modem ? SCC_INT::EXTERNAL_A
                                       : SCC_INT::EXTERNAL_B);
        }
        break;
    default:
        break;
    }
}
static constexpr int get_int_priority(SCC_INT i) {
    switch(i) {
    case SCC_INT::TRANS_B:
        return 1;
    case SCC_INT::EXTERNAL_B:
        return 0;
    case SCC_INT::RECV_AVAIL_B:
    case SCC_INT::RECV_SPECIAL_B:
        return 2;
    case SCC_INT::TRANS_A:
        return 4;
    case SCC_INT::EXTERNAL_A:
        return 3;
    case SCC_INT::RECV_AVAIL_A:
    case SCC_INT::RECV_SPECIAL_A:
        return 5;
    default:
        return -1;
    }
}

void SCC::interrupt(SCC_INT i) {
    if(!MIE) {
        return;
    }
    switch(i) {
    case SCC_INT::NONE:
        break;
    case SCC_INT::EXTERNAL_B:
        printer->external_pending = true;
        break;
    case SCC_INT::TRANS_B:
        printer->trans_pending = true;
        break;
    case SCC_INT::RECV_AVAIL_B:
        printer->recv_avail_pending = true;
        break;
    case SCC_INT::RECV_SPECIAL_B:
        printer->recv_special_pending = true;
        break;
    case SCC_INT::EXTERNAL_A:
        modem->external_pending = true;
        break;
    case SCC_INT::TRANS_A:
        modem->trans_pending = true;
        break;
    case SCC_INT::RECV_AVAIL_A:
        modem->recv_avail_pending = true;
        break;
    case SCC_INT::RECV_SPECIAL_A:
        modem->recv_special_pending = true;
        break;
    }
    if((IUS == SCC_INT::NONE) ||
       (!(DLC && i != SCC_INT::RECV_SPECIAL_A) &&
        get_int_priority(IUS) <= get_int_priority(i))) {
        if(i == SCC_INT::EXTERNAL_A) {
            modem->latch_enable = true;
            modem->latched_rr0 = modem->current_rr0.load();
        } else if(i == SCC_INT::EXTERNAL_B) {
            printer->latch_enable = true;
            printer->latched_rr0 = printer->current_rr0.load();
        }
        IUS = i;
        cpu.irq(4);
    }
}

uint8_t SCC::get_int_vec() {
    return (modem->recv_avail_pending | modem->recv_special_pending) << 5 |
           modem->trans_pending << 4 | modem->external_pending << 3 |
           (printer->recv_special_pending | printer->recv_avail_pending) << 2 |
           printer->trans_pending << 1 | printer->external_pending;
}
uint8_t SCC::interrupt_ack() {
    if(!NV) {
        return get_int_vec();
    }
    return 0;
}

void SCC::write_reg(bool is_modem, int reg, uint8_t value) {
    if(is_modem) {
        modem->write_reg_impl(reg, value);
    } else {
        printer->write_reg_impl(reg, value);
    }
}
uint8_t SCC::read_reg(bool is_modem, int reg) {
    if(is_modem) {
        return modem->read_reg_impl(reg);
    } else {
        return printer->read_reg_impl(reg);
    }
}
void SCC_impl::sync_done() {
    uint16_t crc =
        crc_is_16 ? send_crc16.checksum() : send_crc_ccitt.checksum();
    device->transmit_xd(crc >> 8);
    device->transmit_xd(crc);
    sync_sending = false;
}
// DMA/Sync mode
void SCC::write_all_data(bool port, const std::vector<uint8_t> &values) {
    SCC_impl &impl = port ? *modem : *printer;
    for(uint8_t v : values) {
        impl.send_char = v;
        impl.write_data();
    }
    impl.sync_done();
}
SCC_IOP *scc;
void scc_init() {
    if(scc) {
        delete scc;
    }
    scc = new SCC_IOP();
}