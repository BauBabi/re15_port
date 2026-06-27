void VMANAGER_OBJ_E9C(undefined4 param_1,undefined4 param_2,ushort param_3,uint param_4)

{
  byte bVar1;
  uint uVar2;
  ushort uVar3;
  ushort in_t0;
  
  bVar1 = DAT_800b52a8;
  (&DAT_8008f848)[(param_4 & 0xff) * 0x1a] = 10;
  uVar3 = 0;
  if (bVar1 != 0) {
    uVar2 = 0;
    do {
      uVar3 = uVar3 + 1;
      (&DAT_8008f85f)[uVar2 * 0x34] = (&DAT_8008f85f)[uVar2 * 0x34] & 1;
      uVar2 = (uint)uVar3;
    } while (uVar3 < DAT_800b52a8);
  }
  (&DAT_8008f85f)[(param_4 & 0xff) * 0x34] = 2;
  (&DAT_8008f846)[(param_4 & 0xff) * 0x1a] = 0;
  DAT_8008ff84 = in_t0 | DAT_8008ff84;
  DAT_8008ff88 = param_3 | DAT_8008ff88;
  SPU_VOICE_CHN_NOISE_MODE._0_2_ = in_t0;
  SPU_VOICE_CHN_NOISE_MODE._2_2_ = param_3;
  DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
  DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
  return;
}
