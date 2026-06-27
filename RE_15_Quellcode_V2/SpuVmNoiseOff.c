void SpuVmNoiseOff(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  if (DAT_800b52a8 != 0) {
    iVar1 = 0;
    do {
      if ((&DAT_8008f85f)[(iVar1 >> 0x10) * 0x34] == '\x02') {
        (&DAT_8008f85f)[(uVar2 & 0xff) * 0x34] = 0;
        (&DAT_8008f848)[(uVar2 & 0xff) * 0x1a] = 0;
        SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
        SPU_VOICE_CHN_NOISE_MODE._2_2_ = 0;
      }
      uVar2 = uVar2 + 1;
      iVar1 = uVar2 * 0x10000;
    } while ((int)(uVar2 * 0x10000) >> 0x10 < (int)(uint)DAT_800b52a8);
  }
  return;
}
