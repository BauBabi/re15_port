/* getintr @ 0x8008742c  (Ghidra headless, raw MIPS overlay) */

undefined4 getintr(void)

{
  undefined4 uVar1;
  
  *PTR_CDROM_REG0_800ad0b4 = 1;
  if ((*PTR_CDROM_REG3_800ad0c0 & 7) != 0) {
    uVar1 = BIOS_OBJ_64();
    return uVar1;
  }
  return 0;
}


