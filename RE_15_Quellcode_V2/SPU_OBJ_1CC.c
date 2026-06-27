undefined4 SPU_OBJ_1CC(void)

{
  undefined4 uVar1;
  dword *pdVar2;
  int iVar3;
  int unaff_s0;
  
  DAT_80076d7c = 2;
  DAT_80076d80 = 3;
  DAT_80076d84 = 8;
  DAT_80076d88 = 7;
  SOUND_RAM_DATA_TRANSTER_CTRL = 4;
  SPU_REVERB_OUT_L = 0;
  SPU_REVERB_OUT_R = 0;
  SPU_VOICE_KEY_OFF._0_2_ = 0xffff;
  SPU_VOICE_KEY_OFF._2_2_ = 0xffff;
  SPU_VOICE_CHN_REVERB_MODE._0_2_ = 0;
  SPU_VOICE_CHN_REVERB_MODE._2_2_ = 0;
  if (unaff_s0 == 0) {
    SPU_VOICE_CHN_FM_MODE._0_2_ = 0;
    SPU_VOICE_CHN_FM_MODE._2_2_ = 0;
    SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
    SPU_VOICE_CHN_NOISE_MODE._2_2_ = 0;
    CD_VOL_L = 0;
    CD_VOL_R = 0;
    EXT_VOL_L = 0;
    EXT_VOL_R = 0;
    DAT_80076d58 = 0x200;
    _spu_writeByIO(&DAT_80076d98,0x10);
    iVar3 = 0;
    pdVar2 = &VOICE_00_LEFT_RIGHT;
    do {
      *(undefined2 *)pdVar2 = 0;
      *(undefined2 *)((int)pdVar2 + 2) = 0;
      *(undefined2 *)(pdVar2 + 1) = 0x3fff;
      *(undefined2 *)((int)pdVar2 + 6) = 0x200;
      *(undefined2 *)(pdVar2 + 2) = 0;
      *(undefined2 *)((int)pdVar2 + 10) = 0;
      iVar3 = iVar3 + 1;
      pdVar2 = pdVar2 + 4;
    } while (iVar3 < 0x18);
    SPU_VOICE_KEY_ON._0_2_ = 0xffff;
    SPU_VOICE_KEY_ON._2_2_ = SPU_VOICE_KEY_ON._2_2_ | 0xff;
    uVar1 = SPU_OBJ_314();
    return uVar1;
  }
  DAT_80076d8c = 1;
  SPU_CTRL_REG_CPUCNT = 0xc000;
  DAT_80076d90 = 0;
  DAT_80076d94 = 0;
  return 0;
}
