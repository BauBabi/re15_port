undefined4 timeout(undefined4 param_1)

{
  dword dVar1;
  
  printf("%s timeout:\n",param_1);
  dVar1 = MDEC_REG1;
  printf("\t DMA=(%d,%d), ADDR=(0x%08x->0x%08x)\n",DMA_MDEC_IN_CHCR >> 0x18 & 1,
         DMA_MDEC_OUT_CHCR >> 0x18 & 1,DMA_MDEC_IN_MADR,DMA_MDEC_OUT_MADR);
  printf("\t FIFO=(%d,%d),BUSY=%d,DREQ=(%d,%d),RGB24=%d,STP=%d\n",~dVar1 >> 0x1f,dVar1 >> 0x1e & 1,
         dVar1 >> 0x1d & 1,dVar1 >> 0x1c & 1,dVar1 >> 0x1b & 1,dVar1 >> 0x19 & 1,dVar1 >> 0x17 & 1);
  DMA_MDEC_IN_CHCR = 0;
  DMA_MDEC_OUT_CHCR = 0;
  MDEC_REG1 = 0x60000000;
  return 0;
}
