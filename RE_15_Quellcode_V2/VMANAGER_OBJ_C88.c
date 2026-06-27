void VMANAGER_OBJ_C88(undefined4 param_1,undefined4 param_2,ushort param_3,ushort param_4)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  uint in_t2;
  
  bVar1 = DAT_800b52a8;
  (&DAT_8008f848)[(in_t2 & 0xff) * 0x1a] = 10;
  iVar3 = 0;
  if (bVar1 != 0) {
    iVar2 = 0;
    do {
      iVar2 = (iVar2 >> 0x10) * 0x34;
      iVar3 = iVar3 + 1;
      (&DAT_8008f85f)[iVar2] = (&DAT_8008f85f)[iVar2] & 1;
      iVar2 = iVar3 * 0x10000;
    } while (iVar3 * 0x10000 >> 0x10 < (int)(uint)DAT_800b52a8);
  }
  (&DAT_8008f85f)[(in_t2 & 0xff) * 0x34] = 2;
  DAT_8008ff84 = param_4 | DAT_8008ff84;
  DAT_8008ff88 = param_3 | DAT_8008ff88;
  DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
  DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
  if ((DAT_800b5328 & 4) != 0) {
    DAT_8008ff8c = param_4 | DAT_8008ff8c;
    DAT_8008ff90 = param_3 | DAT_8008ff90;
    VMANAGER_OBJ_E10();
    return;
  }
  SPU_VOICE_CHN_NOISE_MODE._0_2_ = param_4;
  SPU_VOICE_CHN_NOISE_MODE._2_2_ = param_3;
  DAT_8008ff8c = DAT_8008ff8c & ~param_4;
  DAT_8008ff90 = DAT_8008ff90 & ~param_3;
  return;
}
