undefined4 _spu_init(int param_1)

{
  undefined4 uVar1;
  dword *pdVar2;
  int iVar3;
  int local_10;
  
  DMA_DPCR = DMA_DPCR | 0xb0000;
  SPU_CTRL_REG_CPUCNT = 0;
  local_10 = 0;
  DAT_80076d74 = 0;
  DAT_80076d78 = 0;
  DAT_80076d58 = 0;
  do {
    local_10 = local_10 + 1;
  } while (local_10 < 0xf0);
  SPU_MAIN_VOL_L = 0;
  SPU_MAIN_VOL_R = 0;
  DAT_80076d54 = 0;
  while ((SPU_STATUS_REG_SPUSTAT & 0x7ff) != 0) {
    DAT_80076d54 = DAT_80076d54 + 1;
    if (5000 < DAT_80076d54) {
      printf("SPU:T/O [%s]\n","wait (reset)");
      uVar1 = SPU_OBJ_1CC();
      return uVar1;
    }
  }
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
  if (param_1 == 0) {
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
