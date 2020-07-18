#include "serial.hpp"
#include "SDL_net.h"
class SocketSDevice : public SerialDevice {
	const char* addr;
	TCPsocket tcpsock;
	std::atomic<bool> shutdown;
	~SocketSDevice() {
		SDLNet_TCP_Close(tcpsock);
		shutdown = true;
	}
	SocketSDevice(const char* addr) :addr(addr) {
		shutdown = false;
		IPaddress ip;
		if(SDLNetResolveHost(&ip, addr, 12000) == -1 ) {
			throw SDLNet_GetError();
		}
		tcpsock = SDLNetTCP_Open(&ip);
		if( ! tcpsock ) {
			throw SDLNet_GetError();
		}
		if( ! addr ) {
			TCPsocket tcpsock2 = nullptr;
			for(int i = 0;i < 100; ++i) {
				tcpsock2 = SDLNet_TCP_Accept(tcpsock);
				if( ! tcpsock2 ) {
					SDL_Delay(500);
				} else {
					tcpsock = tcpsock2;
					return;
				}
			}
			throw "connection timeout";
		}
	}
	std::vector<uint8_t> Wbuffer;
	void transmit_xd(const std::vector<bool>& out) override {
		if( out.empty() ) {
			SDLNet_TCP_Send(tcpsock, Wbuffer.data(), Wbuffer.size());
			buffer.clear();
		}
		uint16_t bits = connected_to.lock()->decode_bits(out);
		Wbuffer.push_back( bits & 0xff );
	}

	void run(SCC_impl& scc) {
		while(!shutdown) {
			uint8_t r;
			if( SDLNet_TCP_Recv(tcpsock, &r, 1) != 0 ) {
				return;
			}
			std::vector<bool> v, bool parity;
			connected_to.lock()->decode_byte(r, v, parity);
			recieve_xd(v);
		}
		return;		
	}
};
