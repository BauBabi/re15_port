/* CdReady @ 0x80086e0c  (Ghidra headless, raw MIPS overlay) */

/* Possible SEND.OBJ/SendPAD
   Possible GS_106.OBJ/GsSetProjection
   Possible _OP_VDEL.OBJ/__builtin_vec_delete
   Possible _OP_VNEW.OBJ/__builtin_vec_new
   Possible SSNOFF.OBJ/SsSetNoiseOff
   Possible SSQUIT.OBJ/SsQuit */

int CdReady(int mode,u_char *result)

{
  int iVar1;
  
  iVar1 = CD_ready();
  return iVar1;
}


