/* _cwc @ 0x800923bc  (Ghidra headless, raw MIPS overlay) */

void _cwc(undefined4 param_1)

{
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000002;
  *(undefined4 *)PTR_DMA_GPU_MADR_800b27d8 = param_1;
  *(undefined4 *)PTR_DMA_GPU_BCR_800b27dc = 0;
  *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x1000401;
  return;
}


