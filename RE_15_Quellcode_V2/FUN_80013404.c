void FUN_80013404(void)

{
  int iVar1;
  u_char local_10 [8];
  
  SsSetSerialAttr('\0','\0','\x01');
  local_10[0] = 200;
  FUN_80013a88(1,0,0);
  do {
    if (4 < DAT_800bed88) {
      DAT_800bed88 = DAT_800bed88 + 1;
      DAT_800bed9c = 0;
      return;
    }
    DAT_800bed88 = DAT_800bed88 + 1;
    iVar1 = CdControlB('\x0e',local_10,(u_char *)0x0);
  } while ((((iVar1 == 0) || (iVar1 = CdControlB('\r',&DAT_800bed8c,(u_char *)0x0), iVar1 == 0)) ||
           (iVar1 = CdControlB('\x02',"",(u_char *)0x0), iVar1 == 0)) ||
          (iVar1 = CdControl('\x1b',(u_char *)0x0,(u_char *)0x0), iVar1 == 0));
  DAT_800bed9c = 1;
  DAT_800bed84 = 0;
  DAT_800bb4fc = 1;
  return;
}
