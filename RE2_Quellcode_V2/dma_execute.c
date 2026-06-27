/* dma_execute @ 0x80089808  (Ghidra headless, raw MIPS overlay) */

void dma_execute(uint param_1,dword param_2,int param_3,uint param_4,dword param_5,char param_6)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = 0;
  uVar2 = (&DMA_MDEC_IN_CHCR)[param_1 * 4];
  while ((uVar2 & 0x1000000) != 0) {
    if (iVar3 == 0x10000) {
      printf("DMA STATUS ERROR %x\n",(&DMA_MDEC_IN_CHCR)[param_1 * 4]);
      C_011_OBJ_A30();
      return;
    }
    iVar3 = iVar3 + 1;
    uVar2 = (&DMA_MDEC_IN_CHCR)[param_1 * 4];
  }
  uVar2 = 1 << (param_1 & 0x1f);
  if (param_6 != '\x01') {
    PTR_DMA_DICR_800ad168[2] = PTR_DMA_DICR_800ad168[2] & ~(byte)uVar2;
    *(uint *)PTR_DMA_DPCR_800ad164 = *(uint *)PTR_DMA_DPCR_800ad164 | 1 << (param_1 * 4 + 3 & 0x1f);
    (&DMA_MDEC_IN_MADR)[param_1 * 4] = param_2;
    (&DMA_MDEC_IN_BCR)[param_1 * 4] = param_3 << 0x10 | param_4;
    bVar1 = *PTR_CDROM_REG0_800ad14c;
    while ((bVar1 & 0x40) == 0) {
      bVar1 = *PTR_CDROM_REG0_800ad14c;
    }
    (&DMA_MDEC_IN_CHCR)[param_1 * 4] = param_5;
    return;
  }
  C_011_OBJ_A90((byte)PTR_DMA_DICR_800ad168[2] | uVar2);
  return;
}


