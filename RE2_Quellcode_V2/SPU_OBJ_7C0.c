/* SPU_OBJ_7C0 @ 0x80078e20  (Ghidra headless, raw MIPS overlay) */

undefined4 SPU_OBJ_7C0(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint *unaff_s0;
  
  DAT_800ab270 = unaff_s0[-1];
  DAT_800ab274 = (*unaff_s0 >> 6) + (uint)((*unaff_s0 & 0x3f) != 0);
  *(uint *)PTR_DMA_SPU_MADR_800ab220 = DAT_800ab270;
  *(uint *)PTR_DMA_SPU_BCR_800ab224 = DAT_800ab274 << 0x10 | 0x10;
  param_3 = param_3 | 0x201;
  if (DAT_800ab26c == 1) {
    param_3 = 0x1000200;
  }
  *(uint *)PTR_DMA_SPU_CHCR_800ab228 = param_3;
  return 0;
}


