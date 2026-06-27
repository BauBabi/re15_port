void MDEC_out(dword param_1,uint param_2)

{
  MDEC_out_sync();
  DMA_DPCR = DMA_DPCR | 0x88;
  DMA_MDEC_OUT_MADR = param_1;
  DMA_MDEC_OUT_BCR = (param_2 >> 5) << 0x10 | 0x20;
  DMA_MDEC_OUT_CHCR = 0x1000200;
  return;
}
