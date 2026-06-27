void MDEC_in(dword *param_1,uint param_2)

{
  MDEC_in_sync();
  DMA_DPCR = DMA_DPCR | 0x88;
  DMA_MDEC_IN_MADR = (dword)(param_1 + 1);
  DMA_MDEC_IN_BCR = (param_2 >> 5) << 0x10 | 0x20;
  MDEC_REG0 = *param_1;
  DMA_MDEC_IN_CHCR = 0x1000201;
  return;
}
