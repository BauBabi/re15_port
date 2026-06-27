/* DecDCTout @ 0x80094a58  (Ghidra headless, raw MIPS overlay) */

/* Possible SEND.OBJ/SendPAD
   Possible GS_106.OBJ/GsSetProjection
   Possible _OP_VDEL.OBJ/__builtin_vec_delete
   Possible _OP_VNEW.OBJ/__builtin_vec_new
   Possible SSNOFF.OBJ/SsSetNoiseOff
   Possible SSQUIT.OBJ/SsQuit */

void DecDCTout(u_long *buf,int size)

{
  MDEC_out();
  return;
}


