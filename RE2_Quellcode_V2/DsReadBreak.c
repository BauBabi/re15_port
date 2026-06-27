/* DsReadBreak @ 0x8008c6b4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void DsReadBreak(void)

{
  if (DAT_800ad324 == 1) {
    DsFlush();
    DsEndReadySystem();
    if ((DAT_800ad310 & 1) != 0) {
      DsDataCallback(DAT_800ad318);
    }
    DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
  }
  DAT_800ad324 = 0;
  return;
}


