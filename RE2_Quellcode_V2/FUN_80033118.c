/* FUN_80033118 @ 0x80033118  (Ghidra headless, raw MIPS overlay) */

void FUN_80033118(void)

{
  FUN_80033370();
  DecDCToutCallback((func *)0x0);
  DsDataCallback((func *)0x0);
  DsReadyCallback((DslCB)0x0);
  DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
  FUN_80086c1c();
  SsSetSerialAttr('\0','\0','\0');
  return;
}


