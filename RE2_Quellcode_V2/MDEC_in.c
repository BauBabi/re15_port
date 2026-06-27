/* MDEC_in @ 0x80094c34  (Ghidra headless, raw MIPS overlay) */

void MDEC_in(undefined4 *param_1,uint param_2)

{
  MDEC_in_sync();
  *(uint *)PTR_DMA_DPCR_800b2968 = *(uint *)PTR_DMA_DPCR_800b2968 | 0x88;
  *(undefined4 **)PTR_DMA_MDEC_IN_MADR_800b2930 = param_1 + 1;
  *(uint *)PTR_DMA_MDEC_IN_BCR_800b2934 = (param_2 >> 5) << 0x10 | 0x20;
  *(undefined4 *)PTR_MDEC_REG0_800b2960 = *param_1;
  *(undefined4 *)PTR_DMA_MDEC_IN_CHCR_800b2938 = 0x1000201;
  return;
}


