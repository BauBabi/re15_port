/* _spu_FsetPCR @ 0x800790d4  (Ghidra headless, raw MIPS overlay) */

void _spu_FsetPCR(int param_1)

{
  undefined4 uVar1;
  
  *(uint *)PTR_DMA_DPCR_800ab22c = *(uint *)PTR_DMA_DPCR_800ab22c & 0xfff8ffff;
  if (param_1 != 0) {
    uVar1 = *(undefined4 *)PTR_DMA_DPCR_800ab22c;
    SPU_OBJ_AC4(0x30000);
    return;
  }
  *(uint *)PTR_DMA_DPCR_800ab22c = *(uint *)PTR_DMA_DPCR_800ab22c | 0x50000;
  return;
}


