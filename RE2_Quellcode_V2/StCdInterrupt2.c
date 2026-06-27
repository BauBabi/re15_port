/* StCdInterrupt2 @ 0x8008c82c  (Ghidra headless, raw MIPS overlay) */

/* Possible SEND.OBJ/SendPAD
   Possible GS_106.OBJ/GsSetProjection
   Possible _OP_VDEL.OBJ/__builtin_vec_delete
   Possible _OP_VNEW.OBJ/__builtin_vec_new
   Possible SSNOFF.OBJ/SsSetNoiseOff
   Possible SSQUIT.OBJ/SsQuit */

void StCdInterrupt2(void)

{
  StCdInterrupt();
  return;
}


