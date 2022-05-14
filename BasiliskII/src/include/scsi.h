/*
 *  scsi.h - SCSI Manager
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SCSI_H
#define SCSI_H

extern int16_t SCSIReset(void);
extern int16_t SCSIGet(void);
extern int16_t SCSISelect(int id);
extern int16_t SCSICmd(int cmd_length, uint8_t *cmd);
extern int16_t SCSIRead(uint32_t tib);
extern int16_t SCSIWrite(uint32_t tib);
extern int16_t SCSIComplete(uint32_t timeout, uint32_t message, uint32_t stat);
extern uint16_t SCSIStat(void);
extern int16_t SCSIMgrBusy(void);

// System specific and internal functions/data
extern void SCSIInit(void);
extern void SCSIExit(void);

extern void scsi_set_cmd(int cmd_length, uint8_t *cmd);
extern bool scsi_is_target_present(int id);
extern bool scsi_set_target(int id, int lun);
extern bool scsi_send_cmd(size_t data_length, bool reading, int sg_index,
                          uint8_t **sg_ptr, uint32_t *sg_len, uint16_t *stat,
                          uint32_t timeout);

#endif
