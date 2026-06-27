/* CD_getsector @ 0x80088920  (Ghidra headless, raw MIPS overlay) */

undefined4 CD_getsector(undefined4 param_1,uint param_2)

{
  uint uVar1;
  
  *PTR_CDROM_REG0_800ad0b4 = 0;
  *PTR_CDROM_REG3_800ad0c0 = 0x80;
  *(undefined4 *)PTR_CDROM_DELAY_800ad0e8 = 0x20943;
  *(undefined4 *)PTR_COMMON_DELAY_800ad0c4 = 0x1323;
  *(uint *)PTR_DMA_DPCR_800ad0ec = *(uint *)PTR_DMA_DPCR_800ad0ec | 0x8000;
  *(undefined4 *)PTR_DMA_CDROM_MADR_800ad0f0 = param_1;
  *(uint *)PTR_DMA_CDROM_BCR_800ad0f4 = param_2 | 0x10000;
  do {
  } while ((*PTR_CDROM_REG0_800ad0b4 & 0x40) == 0);
  *(undefined4 *)PTR_DMA_CDROM_CHCR_800ad0f8 = 0x11000000;
  uVar1 = *(uint *)PTR_DMA_CDROM_CHCR_800ad0f8;
  while ((uVar1 & 0x1000000) != 0) {
    uVar1 = *(uint *)PTR_DMA_CDROM_CHCR_800ad0f8;
  }
  *(undefined4 *)PTR_COMMON_DELAY_800ad0c4 = 0x1325;
  return 0;
}


