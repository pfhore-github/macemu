Macintosh subchip IO reference

GLU
$00000-$01F00: VIA1 (9-12)
$02000-$03F00: VIA2 (9-12)
$04000-$04006: SCC (1-2)
$06000-$06070: SCSI(handshake) (4-6)
$08000-      : reserved
$10000-$10070: SCSI(4-6)
$14000-$14F37: ASC(0-11)
$16000-$17F00: SWIM(9-12)

VIA1
  $0000 : register B
  $0200 : buffer A with handshake(not implemented)
  $0400 : direction B
  $0600 : direction A
  $0800 : Timer1 Low
  $0A00 : TImer1 High
  $0C00 : Timer1 Latch Low
  $0E00 : Timer1 Latch High
  $1000 : Timer2 Low
  $1200 : Timer2 High
  $1400 : Shift Register
  $1600 : Auxilary CR
  $1800 : Peripheral CR
  $1A00 : IRQ(IRQ vec1 or 6(Quadra only)
  $1C00 : IRQ enable
  $1E00 : register A
    
$02000-$03F00: VIA2
  $2000 : register B
  $0200 : buffer A with handshake(not implemented)
  $0400 : direction B
  $0600 : direction A
  $0800 : Timer1 Low
  $0A00 : TImer1 High
  $0C00 : Timer1 Latch Low
  $0E00 : Timer1 Latch High
  $1000 : Timer2 Low
  $1200 : Timer2 High
  $1400 : Shift Register
  $1600 : Auxilary CR
  $1800 : Peripheral CR
  $1A00 : IRQ
  $1C00 : IRQ enable
  $1E00 : register A
