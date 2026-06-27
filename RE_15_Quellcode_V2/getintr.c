undefined4 getintr(void)

{
  undefined4 uVar1;
  
  CDROM_REG0 = 1;
  if ((CDROM_REG3 & 7) != 0) {
    uVar1 = BIOS_OBJ_64();
    return uVar1;
  }
  return 0;
}
