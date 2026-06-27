/* CD_flush @ 0x800883a4  (Ghidra headless, raw MIPS overlay) */

void CD_flush(void)

{
  byte bVar1;
  
  *PTR_CDROM_REG0_800ad0b4 = 1;
  bVar1 = *PTR_CDROM_REG3_800ad0c0;
  while ((bVar1 & 7) != 0) {
    *PTR_CDROM_REG0_800ad0b4 = 1;
    *PTR_CDROM_REG3_800ad0c0 = 7;
    *PTR_CDROM_REG2_800ad0bc = 7;
    bVar1 = *PTR_CDROM_REG3_800ad0c0;
  }
  DAT_800ad0ce = 0;
  DAT_800ad0cd = 0;
  DAT_800ad0cc = 2;
  *PTR_CDROM_REG0_800ad0b4 = 0;
  *PTR_CDROM_REG3_800ad0c0 = 0;
  *(undefined4 *)PTR_COMMON_DELAY_800ad0c4 = 0x1325;
  return;
}


