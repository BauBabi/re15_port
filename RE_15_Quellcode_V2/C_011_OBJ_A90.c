void C_011_OBJ_A90(undefined1 param_1)

{
  int in_v1;
  int unaff_s0;
  dword unaff_s2;
  int unaff_s3;
  uint unaff_s4;
  dword unaff_s5;
  
  *(undefined1 *)(in_v1 + 2) = param_1;
  DMA_DPCR = DMA_DPCR | 1 << (unaff_s0 * 4 + 3U & 0x1f);
  (&DMA_MDEC_IN_MADR)[unaff_s0 * 4] = unaff_s2;
  (&DMA_MDEC_IN_BCR)[unaff_s0 * 4] = unaff_s3 << 0x10 | unaff_s4;
  do {
  } while ((CDROM_REG0 & 0x40) == 0);
  (&DMA_MDEC_IN_CHCR)[unaff_s0 * 4] = unaff_s5;
  return;
}
