void VMANAGER_OBJ_BF8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined2 param_4)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  ushort in_t1;
  uint in_t2;
  
  SPU_CTRL_REG_CPUCNT = in_t1 & 0xc0ff | ((ushort)DAT_800b5316 - (ushort)DAT_800b5324 & 0x3f) << 8;
  uVar4 = in_t2 & 0xff;
  (&DAT_8008f6ae)[uVar4 * 8] = param_4;
  bVar1 = (&DAT_8008f82c)[uVar4];
  (&DAT_8008f6ac)[uVar4 * 8] = (short)param_2;
  (&DAT_8008f82c)[uVar4] = bVar1 | 3;
  bVar1 = DAT_800b52a8;
  if (uVar4 < 0x10) {
    VMANAGER_OBJ_C88(uVar4,param_2,0,1 << (in_t2 & 0x1f));
    return;
  }
  (&DAT_8008f848)[(in_t2 & 0xff) * 0x1a] = 10;
  iVar5 = 0;
  if (bVar1 != 0) {
    iVar3 = 0;
    do {
      iVar3 = (iVar3 >> 0x10) * 0x34;
      iVar5 = iVar5 + 1;
      (&DAT_8008f85f)[iVar3] = (&DAT_8008f85f)[iVar3] & 1;
      iVar3 = iVar5 * 0x10000;
    } while (iVar5 * 0x10000 >> 0x10 < (int)(uint)DAT_800b52a8);
  }
  (&DAT_8008f85f)[(in_t2 & 0xff) * 0x34] = 2;
  uVar2 = (ushort)(1 << (uVar4 - 0x10 & 0x1f));
  DAT_8008ff88 = uVar2 | DAT_8008ff88;
  DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
  DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
  if ((DAT_800b5328 & 4) != 0) {
    DAT_8008ff90 = uVar2 | DAT_8008ff90;
    VMANAGER_OBJ_E10();
    return;
  }
  SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
  SPU_VOICE_CHN_NOISE_MODE._2_2_ = uVar2;
  DAT_8008ff90 = DAT_8008ff90 & ~uVar2;
  return;
}
