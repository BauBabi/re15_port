/* DsClose @ 0x8008a360  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* Possible SEND.OBJ/SendPAD
   Possible GS_106.OBJ/GsSetProjection
   Possible _OP_VDEL.OBJ/__builtin_vec_delete
   Possible _OP_VNEW.OBJ/__builtin_vec_new
   Possible SSNOFF.OBJ/SsSetNoiseOff
   Possible SSQUIT.OBJ/SsQuit */

void DsClose(void)

{
  DS_close();
  return;
}


