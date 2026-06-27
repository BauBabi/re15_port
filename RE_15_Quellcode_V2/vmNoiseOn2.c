void vmNoiseOn2(uint param_1,undefined4 param_2,undefined2 param_3)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  ushort uVar4;
  
  uVar3 = param_1 & 0xff;
  (&DAT_8008f6ae)[uVar3 * 8] = param_3;
  bVar1 = (&DAT_8008f82c)[uVar3];
  (&DAT_8008f6ac)[uVar3 * 8] = (short)param_2;
  (&DAT_8008f82c)[uVar3] = bVar1 | 3;
  bVar1 = DAT_800b52a8;
  if (uVar3 < 0x10) {
    VMANAGER_OBJ_E9C(uVar3,param_2,0);
    return;
  }
  (&DAT_8008f848)[(param_1 & 0xff) * 0x1a] = 10;
  uVar4 = 0;
  if (bVar1 != 0) {
    uVar2 = 0;
    do {
      uVar4 = uVar4 + 1;
      (&DAT_8008f85f)[uVar2 * 0x34] = (&DAT_8008f85f)[uVar2 * 0x34] & 1;
      uVar2 = (uint)uVar4;
    } while (uVar4 < DAT_800b52a8);
  }
  (&DAT_8008f85f)[(param_1 & 0xff) * 0x34] = 2;
  (&DAT_8008f846)[(param_1 & 0xff) * 0x1a] = 0;
  SPU_VOICE_CHN_NOISE_MODE._2_2_ = (ushort)(1 << (uVar3 - 0x10 & 0x1f));
  DAT_8008ff88 = SPU_VOICE_CHN_NOISE_MODE._2_2_ | DAT_8008ff88;
  SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
  DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
  DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
  return;
}
