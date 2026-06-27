void VMANAGER_OBJ_514(undefined4 param_1,uint param_2,uint param_3)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  undefined2 in_t0;
  uint in_t1;
  
  uVar3 = param_2;
  if ((DAT_800b2678 == 1) && (uVar3 = param_3, param_3 < param_2)) {
    VMANAGER_OBJ_540(param_1,param_2,param_2);
    return;
  }
  uVar2 = in_t1 & 0xffff;
  (&DAT_8008f6b0)[uVar2] = in_t0;
  (&DAT_8008f6ac)[uVar2] = (short)((param_3 * param_3) / 0x3fff);
  (&DAT_8008f6ae)[uVar2] = (short)((uVar3 * uVar3) / 0x3fff);
  (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 7;
  (&DAT_8008f848)[DAT_800b532e * 0x1a] = in_t0;
  (&DAT_8008f85f)[DAT_800b532e * 0x34] = 1;
  uVar3 = (uint)DAT_800b532e;
  if ((int)uVar3 < 0x10) {
    VMANAGER_OBJ_670(&DAT_800b532e,0,1 << (uVar3 & 0x1f));
    return;
  }
  uVar1 = (ushort)(1 << (uVar3 - 0x10 & 0x1f));
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
