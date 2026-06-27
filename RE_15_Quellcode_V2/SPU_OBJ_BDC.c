void SPU_OBJ_BDC(undefined4 param_1,undefined4 param_2,int param_3,dword param_4)

{
  undefined4 local_res0;
  
  for (; local_res0 < 0xf0; local_res0 = local_res0 + 1) {
  }
  SPU_DELAY = SPU_DELAY & 0xf0ffffff | 0x22000000;
  DMA_SPU_MADR = param_4;
  DMA_SPU_BCR = param_3 << 0x10 | 0x10;
  DAT_80076da8 = 1;
  DMA_SPU_CHCR = 0x1000200;
  return;
}
