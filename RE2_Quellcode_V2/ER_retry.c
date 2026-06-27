/* ER_retry @ 0x8008cbc4  (Ghidra headless, raw MIPS overlay) */

void ER_retry(void)

{
  u_char mode;
  u_char com;
  DslLOC *pDVar1;
  
  DsReadyCallback((DslCB)0x0);
  pDVar1 = (DslLOC *)DS_lastpos();
  DAT_800ad32c = DsPosToInt(pDVar1);
  mode = DS_lastmode();
  pDVar1 = (DslLOC *)DS_lastpos();
  com = DS_lastread();
  DsPacket(mode,pDVar1,com,ER_cbsync,-1);
  return;
}


