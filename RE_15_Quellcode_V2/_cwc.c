void _cwc(dword param_1)

{
  GPU_REG1 = 0x4000002;
  DMA_GPU_MADR = param_1;
  DMA_GPU_BCR = 0;
  DMA_GPU_CHCR = 0x1000401;
  return;
}
