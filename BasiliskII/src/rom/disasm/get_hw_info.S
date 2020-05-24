02f64:	 bral 0x4b6c			; 60ff 0000 1c06 
02f6a:	 tstb %d2			; 4a02           
02f6c:	 beqs 0x2f9e			; 6730           
02f6e:	 lea %pc@(0x325c),%a0		; 41fa 02ec      
02f72:	 addqw #4,%a0			; 5848           
02f74:	 movel %a0@,%d0			; 2010           
02f76:	 beqs 0x2f9e			; 6726           
02f78:	 lea %a0@(0000000000000000,%d0:l),%a1; 43f0 0800      
02f7c:	 cmpw %a1@(18),%d2		; b469 0012      
02f80:	 bnes 0x2f72			; 66f0           
02f82:	 nop				; 4e71           
02f84:	 nop				; 4e71           
02f86:	 moveal %a1@,%a0		; 2051           
02f88:	 addal %a1,%a0			; d1c9           
02f8a:	 movel %a1@(24),%d0		; 2029 0018      
02f8e:	 beqs 0x2fe4			; 6754           
02f90:	 movel %a1@(16),%d2		; 2429 0010      
02f94:	 braw 0x4b0c			; 6000       
02f9e:	 movel %fp,%d0			; 200e           
02fa0:	 lea %pc@(0x3240),%a1		; 43fa 029e      
02fa4:	 moveal %a1,%a0			; 2049           
02fa6:	 addal %a1@+,%a0		; d1d9           
02fa8:	 moveal %a0@(-8),%a2		; 2468 fff8      
02fac:	 jmp %a0@(0000000000000000,%a2:l); 4ef0 a800      
02fb0:	 moveal %d0,%a2			; 2440           
02fb2:	 movel %a0@(-20),%d0		; 2028 ffec      
02fb6:	 call 0x47cc			; 4dfa 0006 4efa 1810      
02fbe:	 moveal %a2,%fp			; 2c4a           
02fc0:	 moveb %a0@(-4),%d2		; 1428 fffc      
02fc4:	 lea %pc@(0x3260),%a0		; 41fa 029a      
02fc8:	 movel %a0@+,%d0		; 2018           
02fca:	 beqs 0x2f9e			; 67d2           
02fcc:	 lea %a0@(fffffffffffffffc,%d0:l),%a1; 43f0 08fc      
02fd0:	 cmpb %a1@(19),%d2		; b429 0013      
02fd4:	 bnes 0x2fc8			; 66f2           
02fd6:	 movel %d1,%d0			; 2001           
02fd8:	 andl %a1@(32),%d0		; c0a9 0020      
02fdc:	 cmpl %a1@(36),%d0		; b0a9 0024      
02fe0:	 bnes 0x2fc8			; 66e6           
02fe2:	 bras 0x2f82			; 609e           
02fe4:	 movel %a0@(-20),%d0		; 2028 ffec      
02fe8:	 moveal %fp,%a0			; 204e           
02fea:	 btst #13,%d0			; 0800 000d      
02fee:	 beqs 0x300c			; 671c           
02ff0:	 moveal %a1@,%a2		; 2451           
02ff2:	 moveal %a1@(0000000000000034,%a2:l),%a2; 2471 a834      
02ff6:	 lea %a2@(19),%a2		; 45ea 0013      
02ffa:	 call 0x46d8			; 4dfa 0006 4efa 16d8      
03002:	 beqs 0x300c			; 6708           
03004:	 bclr #13,%d0			; 0880 000d      
03008:	 bclr #14,%d0			; 0880 000e      
0300c:	 bclr #11,%d0			; 0880 000b      
03010:	 beqs 0x302a			; 6718           
03012:	 moveal %a1@,%a2		; 2451           
03014:	 moveal %a1@(000000000000002c,%a2:l),%a2; 2471 a82c      
03018:	 lea %a2@(7168),%a2		; 45ea 1c00      
0301c:	 call 0x46aa			; 4dfa 0006 4efa 1688      
03024:	 bnes 0x302a			; 6604           
03026:	 bset #11,%d0			; 08c0 000b      
0302a:	 bclr #16,%d0			; 0880 0010      
0302e:	 beqs 0x3048			; 6718           
03030:	 moveal %a1@,%a2		; 2451           
03032:	 moveal %a1@(0000000000000040,%a2:l),%a2; 2471 a840      
03036:	 call 0x477c			; 4dfa 0006 4efa 1740      
0303e:	 bnes 0x3048			; 6608           
03040:	 bclr #5,%d0			; 0880 0005      
03044:	 bset #16,%d0			; 08c0 0010      
03048:	 bclr #17,%d0			; 0880 0011      
0304c:	 beqs 0x306a			; 671c           
0304e:	 moveal %a1@,%a2		; 2451           
03050:	 moveal %a1@(0000000000000044,%a2:l),%a2; 2471 a844      
03054:	 call 0x477c			; 4dfa 0006 4efa 1722      
0305c:	 bnes 0x306a			; 660c           
0305e:	 bset #17,%d0			; 08c0 0011      
03062:	 bclr #3,%d0			; 0880 0003      
03066:	 bclr #4,%d0			; 0880 0004      
0306a:	 bclr #15,%d0			; 0880 000f      
0306e:	 beqs 0x3090			; 6720           
03070:	 moveal %a1@,%a2		; 2451           
03072:	 moveal %a1@(000000000000003c,%a2:l),%a2; 2471 a83c      
03076:	 call 0x4700			; 4dfa 0006 4efa 1684      
0307e:	 bnes 0x3090			; 6610           
03080:	 bset #15,%d0			; 08c0 000f      
03084:	 bclr #8,%d0			; 0880 0008      
03088:	 bclr #9,%d0			; 0880 0009      
0308c:	 bclr #10,%d0			; 0880 000a      
03090:	 bral 0x4a96			; 60ff 0000 1a04 
03096:	 addal %a0@,%a0			; d1d0           
03098:	 braw 0x2f90			; 6000 fef6      
0309c:	 movel %a1@(28),%d1		; 2229 001c      
030a0:	 bnes 0x30a6			; 6604           
030a2:	 movel %a0@(-16),%d1		; 2228 fff0      
030a6:	 bclr #27,%d7			; 0887 001b      
030aa:	 ret				; 4ed6           
