undefined4 cd_read_retry(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  CdlLOC *pCVar3;
  int iVar4;
  uint uVar5;
  u_char local_18 [8];
  
  CdSyncCallback((CdlCB)0x0);
  CdReadyCallback((CdlCB)0x0);
  uVar1 = CdStatus();
  if ((uVar1 & 0x10) != 0) {
    uVar1 = FUN_80061fc0(0xffffffff);
    if ((uVar1 & 0x3f) == 0) {
      puts("CdRead: Shell open...\n");
    }
    CdControlF('\x01',(u_char *)0x0);
    DAT_80078814 = FUN_80061fc0(0xffffffff);
    DAT_8007880c = 0xffffffff;
    uVar2 = CDREAD_OBJ_3DC();
    return uVar2;
  }
  if (param_1 != 0) {
    puts("CdRead: retry...\n");
    CdControl('\t',(u_char *)0x0,(u_char *)0x0);
    pCVar3 = CdLastPos();
    iVar4 = CdControl('\x02',&pCVar3->minute,(u_char *)0x0);
    if (iVar4 == 0) {
      DAT_8007880c = 0xffffffff;
      uVar2 = CDREAD_OBJ_3E4();
      return uVar2;
    }
  }
  CdFlush();
  local_18[0] = (u_char)DAT_80078804;
  uVar5 = DAT_80078804 & 0xff;
  uVar1 = CdMode();
  if (((uVar5 != uVar1) || (param_1 != 0)) &&
     (iVar4 = CdControl('\x0e',local_18,(u_char *)0x0), iVar4 == 0)) {
    DAT_8007880c = 0xffffffff;
    uVar2 = CDREAD_OBJ_3DC();
    return uVar2;
  }
  pCVar3 = CdLastPos();
  DAT_80078818 = CdPosToInt(pCVar3);
  CdReadyCallback(cb_read);
  DAT_80078800 = DAT_800787fc;
  CdControlF('\x06',(u_char *)0x0);
  DAT_8007880c = DAT_800787f8;
  DAT_80078810 = FUN_80061fc0(0xffffffff);
  return DAT_8007880c;
}
