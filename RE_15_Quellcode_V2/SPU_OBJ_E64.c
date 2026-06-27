undefined4 SPU_OBJ_E64(uint *param_1,undefined4 param_2,dword *param_3)

{
  uint in_v0;
  uint in_v1;
  
  *param_1 = in_v0 | in_v1;
  DAT_80076db0 = (param_3[1] >> 6) + (uint)((param_3[1] & 0x3f) != 0);
  DMA_SPU_MADR = *param_3;
  DMA_SPU_BCR = DAT_80076db0 * 0x10000 | 0x10;
  DMA_SPU_CHCR = 0x1000201;
  if (DAT_80076da8 == 1) {
    DMA_SPU_CHCR = 0x1000200;
  }
  DAT_80076dac = *param_3;
  return 0;
}
