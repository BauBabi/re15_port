/* DsEndReadySystem @ 0x8008c8d8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void DsEndReadySystem(void)

{
  if (DAT_800ad344 == 1) {
    DsReadyCallback(DAT_800ad340);
    DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
  }
  DAT_800ad344 = 0;
  return;
}


