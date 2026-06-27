void FUN_8002a8d0(void)

{
  int iVar1;
  
  FUN_8002ac38();
  while (iVar1 = CdSync(0,(u_char *)0x0), iVar1 != 2) {
    do {
      iVar1 = FUN_8002adb0();
    } while (iVar1 == 0);
  }
  DecDCToutCallback((func *)0x0);
  CdDataCallback((func *)0x0);
  CdReadyCallback((CdlCB)0x0);
  FUN_80064768();
  SsSetSerialAttr('\0','\0','\0');
  if (iRam0000003c != 0) {
    FUN_8002a16c(0x140,0);
    FUN_8002ad64();
  }
  return;
}
