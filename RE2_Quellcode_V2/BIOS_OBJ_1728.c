/* BIOS_OBJ_1728 @ 0x80088b54  (Ghidra headless, raw MIPS overlay) */

void BIOS_OBJ_1728(void)

{
  uint uVar1;
  undefined1 *unaff_s1;
  undefined1 unaff_s2;
  undefined1 *unaff_s3;
  
  while( true ) {
    uVar1 = getintr();
    if (uVar1 == 0) break;
    if (((uVar1 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
      (*DAT_800acdf0)(*unaff_s1,&DAT_800c3d48);
    }
    if (((uVar1 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
      (*DAT_800acdec)(*unaff_s3,&DAT_800c3d40);
    }
  }
  *PTR_CDROM_REG0_800ad0b4 = unaff_s2;
  return;
}


