/* C_011_OBJ_A30 @ 0x80089894  (Ghidra headless, raw MIPS overlay) */

void C_011_OBJ_A30(void)

{
  byte bVar1;
  int in_v0;
  uint uVar2;
  uint unaff_s0;
  dword unaff_s2;
  int unaff_s3;
  uint unaff_s4;
  dword unaff_s5;
  
  uVar2 = 1 << (unaff_s0 & 0x1f);
  if (in_v0 != 1) {
    PTR_DMA_DICR_800ad168[2] = PTR_DMA_DICR_800ad168[2] & ~(byte)uVar2;
    *(uint *)PTR_DMA_DPCR_800ad164 = *(uint *)PTR_DMA_DPCR_800ad164 | 1 << (unaff_s0 * 4 + 3 & 0x1f)
    ;
    (&DMA_MDEC_IN_MADR)[unaff_s0 * 4] = unaff_s2;
    (&DMA_MDEC_IN_BCR)[unaff_s0 * 4] = unaff_s3 << 0x10 | unaff_s4;
    bVar1 = *PTR_CDROM_REG0_800ad14c;
    while ((bVar1 & 0x40) == 0) {
      bVar1 = *PTR_CDROM_REG0_800ad14c;
    }
    (&DMA_MDEC_IN_CHCR)[unaff_s0 * 4] = unaff_s5;
    return;
  }
  C_011_OBJ_A90((byte)PTR_DMA_DICR_800ad168[2] | uVar2);
  return;
}


