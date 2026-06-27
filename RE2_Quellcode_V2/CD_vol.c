/* CD_vol @ 0x8008831c  (Ghidra headless, raw MIPS overlay) */

undefined4 CD_vol(undefined1 *param_1)

{
  *PTR_CDROM_REG0_800ad0b4 = 2;
  *PTR_CDROM_REG2_800ad0bc = *param_1;
  *PTR_CDROM_REG3_800ad0c0 = param_1[1];
  *PTR_CDROM_REG0_800ad0b4 = 3;
  *PTR_CDROM_REG1_800ad0b8 = param_1[2];
  *PTR_CDROM_REG2_800ad0bc = param_1[3];
  *PTR_CDROM_REG3_800ad0c0 = 0x20;
  return 0;
}


