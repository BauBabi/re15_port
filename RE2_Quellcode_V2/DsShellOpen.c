/* DsShellOpen @ 0x8008ac84  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* Possible SEND.OBJ/SendPAD
   Possible GS_106.OBJ/GsSetProjection
   Possible _OP_VDEL.OBJ/__builtin_vec_delete
   Possible _OP_VNEW.OBJ/__builtin_vec_new
   Possible SSNOFF.OBJ/SsSetNoiseOff
   Possible SSQUIT.OBJ/SsQuit */

int DsShellOpen(void)

{
  int iVar1;
  
  iVar1 = DS_shell_open();
  return iVar1;
}


