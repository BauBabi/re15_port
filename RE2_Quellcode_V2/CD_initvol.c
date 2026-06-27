/* CD_initvol @ 0x80088484  (Ghidra headless, raw MIPS overlay) */

undefined4 CD_initvol(void)

{
  undefined *puVar1;
  
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ad0c8;
  if ((*(short *)(PTR_VOICE_00_LEFT_RIGHT_800ad0c8 + 0x1b8) == 0) &&
     (*(short *)(PTR_VOICE_00_LEFT_RIGHT_800ad0c8 + 0x1ba) == 0)) {
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ad0c8 + 0x180) = 0x3fff;
    *(undefined2 *)(puVar1 + 0x182) = 0x3fff;
  }
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ad0c8;
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ad0c8 + 0x1b0) = 0x3fff;
  *(undefined2 *)(puVar1 + 0x1b2) = 0x3fff;
  *(undefined2 *)(puVar1 + 0x1aa) = 0xc001;
  *PTR_CDROM_REG0_800ad0b4 = 2;
  *PTR_CDROM_REG2_800ad0bc = 0x80;
  *PTR_CDROM_REG3_800ad0c0 = 0;
  *PTR_CDROM_REG0_800ad0b4 = 3;
  *PTR_CDROM_REG1_800ad0b8 = 0x80;
  *PTR_CDROM_REG2_800ad0bc = 0;
  *PTR_CDROM_REG3_800ad0c0 = 0x20;
  return 0;
}


