void cb_read(char param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  CdlLOC aCStack_20 [4];
  
  if (param_1 == '\x01') {
    if (0 < DAT_8007880c) {
      if (DAT_80078808 == 0x200) {
        CdGetSector(aCStack_20,3);
        iVar1 = CdPosToInt(aCStack_20);
        if (iVar1 != DAT_80078818) {
          puts("CdRead: sector error\n");
          DAT_8007880c = -1;
        }
      }
      CdGetSector(DAT_80078800,DAT_80078808);
      DAT_80078800 = (void *)(DAT_80078808 * 4 + (int)DAT_80078800);
      DAT_8007880c = DAT_8007880c + -1;
      DAT_80078818 = DAT_80078818 + 1;
      CDREAD_OBJ_114();
      return;
    }
  }
  else {
    DAT_8007880c = -1;
  }
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
      (*DAT_800787f4)(uVar2,param_2);
    }
  }
  return;
}
