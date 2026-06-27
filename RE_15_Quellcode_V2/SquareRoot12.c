long SquareRoot12(long a)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  
  gte_ldLZCS(a);
  uVar1 = gte_stLZCR();
  if (uVar1 == 0x20) {
    return 0;
  }
  uVar1 = uVar1 & 0xfffffffe;
  uVar2 = (int)(0x13 - uVar1) >> 1;
  if ((int)(uVar1 - 0x18) < 0) {
    iVar3 = a >> (0x18 - uVar1 & 0x1f);
  }
  else {
    iVar3 = a << (uVar1 - 0x18 & 0x1f);
  }
  if (-1 < (int)uVar2) {
    return (int)*(short *)(&DAT_8007d984 + (iVar3 + -0x40) * 2) << (uVar2 & 0x1f);
  }
  return (uint)(int)*(short *)(&DAT_8007d984 + (iVar3 + -0x40) * 2) >> (-uVar2 & 0x1f);
}
