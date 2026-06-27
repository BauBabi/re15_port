undefined4 _spu_t(int param_1,uint param_2,uint param_3)

{
  undefined4 uVar1;
  int iVar2;
  ushort uVar3;
  
  if (param_1 == 1) {
    DAT_80076da8 = 0;
    if (SOUND_RAM_DATA_TRANSFER_ADDR != DAT_80076d58) {
      iVar2 = 1;
      do {
        if (0xf00 < iVar2) {
          DAT_80076da8 = 0;
          return 0xfffffffe;
        }
        iVar2 = iVar2 + 1;
      } while (SOUND_RAM_DATA_TRANSFER_ADDR != DAT_80076d58);
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf | 0x20;
    uVar1 = SPU_OBJ_EFC();
    return uVar1;
  }
  if (param_1 < 2) {
    if (param_1 != 0) {
      uVar1 = SPU_OBJ_EFC();
      return uVar1;
    }
    DAT_80076da8 = 1;
    if (SOUND_RAM_DATA_TRANSFER_ADDR != DAT_80076d58) {
      iVar2 = 1;
      do {
        if (0xf00 < iVar2) {
          DAT_80076da8 = 1;
          return 0xfffffffe;
        }
        iVar2 = iVar2 + 1;
      } while (SOUND_RAM_DATA_TRANSFER_ADDR != DAT_80076d58);
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 0x30;
    uVar1 = SPU_OBJ_EFC();
    return uVar1;
  }
  if (param_1 == 2) {
    DAT_80076d58 = (word)(param_2 >> (DAT_80076d80 & 0x1f));
    SOUND_RAM_DATA_TRANSFER_ADDR = DAT_80076d58;
    uVar1 = SPU_OBJ_EFC();
    return uVar1;
  }
  if (param_1 == 3) {
    uVar3 = 0x20;
    if (DAT_80076da8 == 1) {
      uVar3 = 0x30;
    }
    iVar2 = 1;
    while ((SPU_CTRL_REG_CPUCNT & 0x30) != uVar3) {
      if (0xf00 < iVar2) {
        return 0xfffffffe;
      }
      iVar2 = iVar2 + 1;
    }
    if (DAT_80076da8 != 1) {
      DAT_80076db0 = (param_3 >> 6) + (uint)((param_3 & 0x3f) != 0);
      DMA_SPU_CHCR = 0x1000201;
      if (DAT_80076da8 == 1) {
        DMA_SPU_CHCR = 0x1000200;
      }
      DMA_SPU_BCR = DAT_80076db0 * 0x10000 | 0x10;
      DMA_SPU_MADR = param_2;
      SPU_DELAY = SPU_DELAY & 0xf0ffffff | 0x20000000;
      DAT_80076dac = param_2;
      return 0;
    }
    uVar1 = SPU_OBJ_E64();
    return uVar1;
  }
  uVar1 = SPU_OBJ_EFC();
  return uVar1;
}
