bool FUN_8004eae4(void)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  
  if ((&DAT_800b10ae)[(uint)DAT_800b25c8 * 4] == '\x02') {
    uVar2 = DAT_800b25c8 - 1;
  }
  else {
    uVar2 = (uint)DAT_800b25c8;
  }
  iVar3 = (uVar2 & 0xff) * 4;
  cVar1 = (&DAT_800b10ad)[iVar3];
  if (cVar1 != '\0') {
    (&DAT_800b10ad)[iVar3] = cVar1 + -1;
  }
  return cVar1 != '\0';
}
