/* callback @ 0x80088b20  (Ghidra headless, raw MIPS overlay) */

void callback(void)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *PTR_CDROM_REG0_800ad0b4;
  while( true ) {
    uVar2 = getintr();
    if (uVar2 == 0) break;
    if (((uVar2 & 4) != 0) && (DAT_800acdf0 != (code *)0x0)) {
      (*DAT_800acdf0)(DAT_800ad0cd,&DAT_800c3d48);
    }
    if (((uVar2 & 2) != 0) && (DAT_800acdec != (code *)0x0)) {
      (*DAT_800acdec)(DAT_800ad0cc,&DAT_800c3d40);
    }
  }
  *PTR_CDROM_REG0_800ad0b4 = bVar1 & 3;
  return;
}


