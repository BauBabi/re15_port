/* MDEC_out @ 0x80094cc8  (Ghidra headless, raw MIPS overlay) */

void MDEC_out(undefined4 param_1,uint param_2)

{
  MDEC_out_sync();
  *(uint *)PTR_DMA_DPCR_800b2968 = *(uint *)PTR_DMA_DPCR_800b2968 | 0x88;
  *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944 = 0;
  *(undefined4 *)PTR_DMA_MDEC_OUT_MADR_800b293c = param_1;
  *(uint *)PTR_DMA_MDEC_OUT_BCR_800b2940 = (param_2 >> 5) << 0x10 | 0x20;
  *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944 = 0x1000200;
  return;
}


