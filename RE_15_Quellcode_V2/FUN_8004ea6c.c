bool FUN_8004ea6c(void)

{
  bool bVar1;
  uint uVar2;
  
  if ((&DAT_800b10ae)[(uint)DAT_800b25c8 * 4] == '\x02') {
    uVar2 = DAT_800b25c8 - 1;
  }
  else {
    uVar2 = (uint)DAT_800b25c8;
  }
  if ((uVar2 & 0xff) == 0x80) {
    bVar1 = false;
  }
  else {
    bVar1 = (&DAT_800b10ad)[(uVar2 & 0xff) * 4] != '\0';
  }
  return bVar1;
}
