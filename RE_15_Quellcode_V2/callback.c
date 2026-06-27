void callback(void)

{
  uint uVar1;
  byte bVar2;
  
  bVar2 = CDROM_REG0 & 3;
  while( true ) {
    uVar1 = getintr();
    if (uVar1 == 0) break;
    if (((uVar1 & 4) != 0) && (DAT_800788a8 != (code *)0x0)) {
      (*DAT_800788a8)(DAT_80078b81,&DAT_8008fe00);
    }
    if (((uVar1 & 2) != 0) && (DAT_800788a4 != (code *)0x0)) {
      (*DAT_800788a4)(DAT_80078b80,&DAT_8008fdf8);
    }
  }
  CDROM_REG0 = bVar2;
  return;
}
