void C_011_OBJ_A30(void)

{
  int in_v0;
  uint uVar1;
  uint unaff_s0;
  dword unaff_s2;
  int unaff_s3;
  uint unaff_s4;
  dword unaff_s5;
  
  uVar1 = 1 << (unaff_s0 & 0x1f);
  if (in_v0 == 1) {
    C_011_OBJ_A90(DMA_DICR._2_1_ | uVar1);
    return;
  }
  DMA_DICR._2_1_ = DMA_DICR._2_1_ & ~(byte)uVar1;
  DMA_DPCR = DMA_DPCR | 1 << (unaff_s0 * 4 + 3 & 0x1f);
  (&DMA_MDEC_IN_MADR)[unaff_s0 * 4] = unaff_s2;
  (&DMA_MDEC_IN_BCR)[unaff_s0 * 4] = unaff_s3 << 0x10 | unaff_s4;
  do {
  } while ((CDROM_REG0 & 0x40) == 0);
  (&DMA_MDEC_IN_CHCR)[unaff_s0 * 4] = unaff_s5;
  return;
}
