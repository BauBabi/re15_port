void VMANAGER_OBJ_4A4(undefined4 param_1,uint param_2,int param_3)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  undefined2 in_t0;
  uint in_t1;
  
  uVar2 = (uint)DAT_800b5319;
  if (uVar2 < 0x40) {
    VMANAGER_OBJ_514(param_1,(param_2 * uVar2) / 0x3f);
    return;
  }
  uVar4 = (param_3 * (0x7f - uVar2)) / 0x3f;
  uVar2 = param_2;
  if ((DAT_800b2678 == 1) && (uVar2 = uVar4, uVar4 < param_2)) {
    VMANAGER_OBJ_540(param_1,param_2,param_2);
    return;
  }
  uVar3 = in_t1 & 0xffff;
  (&DAT_8008f6b0)[uVar3] = in_t0;
  (&DAT_8008f6ac)[uVar3] = (short)((uVar4 * uVar4) / 0x3fff);
  (&DAT_8008f6ae)[uVar3] = (short)((uVar2 * uVar2) / 0x3fff);
  (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 7;
  (&DAT_8008f848)[DAT_800b532e * 0x1a] = in_t0;
  (&DAT_8008f85f)[DAT_800b532e * 0x34] = 1;
  uVar2 = (uint)DAT_800b532e;
  if ((int)uVar2 < 0x10) {
    VMANAGER_OBJ_670(&DAT_800b532e,0,1 << (uVar2 & 0x1f));
    return;
  }
  uVar1 = (ushort)(1 << (uVar2 - 0x10 & 0x1f));
  if ((DAT_800b5328 & 4) == 0) {
    DAT_8008ff90 = DAT_8008ff90 & ~uVar1;
    DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
    DAT_8008ff88 = uVar1 | DAT_8008ff88;
    DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
    return;
  }
  DAT_8008ff90 = uVar1 | DAT_8008ff90;
  VMANAGER_OBJ_6E8();
  return;
}
