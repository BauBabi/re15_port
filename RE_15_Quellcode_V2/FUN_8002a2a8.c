undefined4 FUN_8002a2a8(int param_1,func *param_2)

{
  int iVar1;
  u_long uVar2;
  u_long *local_18;
  u_long *local_14;
  
  uRam0000000c = (uint)*(ushort *)(&DAT_80073c34 + param_1 * 0xc);
  uRam00000008 = 0x1f004;
  uRam00000004 = 0;
  uRam00000000 = 0;
  uRam00000014 = 0;
  iRam00000010 = 0;
  DecDCTReset(0);
  if (param_2 == (func *)0x0) {
    param_2 = (func *)&LAB_8002aa54;
  }
  DecDCToutCallback(param_2);
  StSetRing((u_long *)0x4,0x12);
  StSetStream(1,2,0xfffffff,(func1 *)0x0,(func2 *)0x0);
  DAT_800b23e8 = FUN_800130c4(*(undefined4 *)(&DAT_80073c30 + param_1 * 0xc),&DAT_800b8984);
  uRam00000040 = DAT_800b23e8;
  CdIntToPos(DAT_800b23e8,(CdlLOC *)&DAT_800b3f80);
  FUN_8002ac38();
  DAT_800b25b5 = 1;
  DAT_800b25b4 = 1;
  do {
    while (iVar1 = CdSync(0,(u_char *)0x0), iVar1 != 2) {
      do {
        iVar1 = FUN_8002adf4();
      } while (iVar1 == 0);
    }
    iVar1 = CdControlB('\r',&DAT_800b25b4,(u_char *)0x0);
  } while (((iVar1 == 0) || (iVar1 = CdControlB('\x15',"",(u_char *)0x0), iVar1 == 0)) ||
          (iVar1 = CdRead2(0x1c8), iVar1 == 0));
  do {
    iVar1 = CdControlB('\x11',(u_char *)0x0,(u_char *)0x0);
  } while (iVar1 == 0);
  DAT_800b23e8 = CdPosToInt((CdlLOC *)0x5);
  if (uRam00000040 < 0x65) {
    if (DAT_800b23e8 == 0) {
      return 1;
    }
  }
  else if (DAT_800b23e8 < uRam00000040 - 100) {
    return 1;
  }
  if (DAT_800b23e8 <= uRam00000040 + 5) {
    DAT_800b52d4 = 0;
    do {
      uVar2 = StGetNext(&local_18,&local_14);
      if (uVar2 == 0) {
        FUN_8002abec();
        uRam00000014 = *local_14;
        uRam00000018 = local_14[1];
        uRam0000001c = local_14[2];
        uRam00000020 = local_14[3];
        uRam00000028 = local_14[5];
        uRam0000002c = local_14[6];
        uRam00000030 = local_14[7];
        uRam00000008 = (local_14[4] & 0xffff) + 8 & 0xfff0;
        uRam0000000c = (local_14[4] >> 0x10) + 8 & 0xfff0;
        uRam00000024 = CONCAT22((short)uRam0000000c,(short)uRam00000008);
        DecDCTvlc(local_18,*(u_long **)(iRam00000010 * 4));
        StFreeRing(local_18);
        return 0;
      }
      FUN_80061fc0(0);
      DAT_800b52d4 = DAT_800b52d4 + 1;
    } while (DAT_800b52d4 < 0xb5);
  }
  return 1;
}
