bool CDREAD_OBJ_468(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  
  DAT_80078804 = DAT_80078804 | 0x20;
  DAT_800787f8 = param_4;
  DAT_800787fc = param_2;
  DAT_8007881c = CdSyncCallback((CdlCB)0x0);
  DAT_80078820 = CdReadyCallback((CdlCB)0x0);
  DAT_80078814 = FUN_80061fc0(0xffffffff);
  uVar1 = CdStatus();
  if ((uVar1 & 0xe0) != 0) {
    CdControlB('\t',(u_char *)0x0,(u_char *)0x0);
  }
  iVar2 = cd_read_retry(0);
  return 0 < iVar2;
}
