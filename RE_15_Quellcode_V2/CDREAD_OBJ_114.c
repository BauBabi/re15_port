void CDREAD_OBJ_114(void)

{
  int iVar1;
  undefined4 uVar2;
  
  DAT_80078810 = FUN_80061fc0(0xffffffff);
  if (DAT_8007880c < 0) {
    cd_read_retry(1);
  }
  iVar1 = FUN_80061fc0(0xffffffff);
  if (DAT_80078814 + 0x4b0 < iVar1) {
    DAT_8007880c = -1;
  }
  if ((DAT_8007880c == 0) || (iVar1 = FUN_80061fc0(0xffffffff), DAT_80078814 + 0x4b0 < iVar1)) {
    CdSyncCallback(DAT_8007881c);
    CdReadyCallback(DAT_80078820);
    CdControl('\t',(u_char *)0x0,(u_char *)0x0);
    if (DAT_800787f4 != (code *)0x0) {
      uVar2 = 5;
      if (DAT_8007880c == 0) {
        uVar2 = 2;
      }
      (*DAT_800787f4)(uVar2);
    }
  }
  return;
}
