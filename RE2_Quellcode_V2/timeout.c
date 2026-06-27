/* timeout @ 0x80094ea0  (Ghidra headless, raw MIPS overlay) */

undefined4 timeout(undefined4 param_1)

{
  undefined4 uVar1;
  uint uVar2;
  
  printf("%s timeout:\n",param_1);
  uVar2 = *(uint *)PTR_MDEC_REG1_800b2964;
  printf("\t DMA=(%d,%d), ADDR=(0x%08x->0x%08x)\n",
         *(uint *)PTR_DMA_MDEC_IN_CHCR_800b2938 >> 0x18 & 1,
         *(uint *)PTR_DMA_MDEC_OUT_CHCR_800b2944 >> 0x18 & 1,
         *(undefined4 *)PTR_DMA_MDEC_IN_MADR_800b2930,*(undefined4 *)PTR_DMA_MDEC_OUT_MADR_800b293c)
  ;
  printf("\t FIFO=(%d,%d),BUSY=%d,DREQ=(%d,%d),RGB24=%d,STP=%d\n",~uVar2 >> 0x1f,uVar2 >> 0x1e & 1,
         uVar2 >> 0x1d & 1,uVar2 >> 0x1c & 1,uVar2 >> 0x1b & 1,uVar2 >> 0x19 & 1,uVar2 >> 0x17 & 1);
  *(undefined4 *)PTR_MDEC_REG1_800b2964 = 0x80000000;
  *(undefined4 *)PTR_DMA_MDEC_IN_CHCR_800b2938 = 0;
  *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944 = 0;
  uVar1 = *(undefined4 *)PTR_DMA_MDEC_OUT_CHCR_800b2944;
  *(undefined4 *)PTR_MDEC_REG1_800b2964 = 0x60000000;
  return 0;
}


