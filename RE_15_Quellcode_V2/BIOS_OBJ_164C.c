void BIOS_OBJ_164C(void)

{
  uint uVar1;
  byte unaff_s1;
  undefined1 *unaff_s2;
  
  while( true ) {
    uVar1 = getintr();
    if (uVar1 == 0) break;
    if (((uVar1 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
      (*DAT_800788a8)(*unaff_s2,&DAT_8008fe00);
    }
    if (((uVar1 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
      (*DAT_800788a4)(DAT_80078b80,&DAT_8008fdf8);
    }
  }
  CDROM_REG0 = unaff_s1;
  return;
}
