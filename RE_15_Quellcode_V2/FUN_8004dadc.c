void FUN_8004dadc(void)

{
  int iVar1;
  
  iVar1 = FUN_8004df2c();
  if (-1 < iVar1 << 0x18) {
    FUN_8004947c((iVar1 + 1U & 0xff) - 1 & 0xffff);
    iVar1 = ((iVar1 + 1U & 0xff) - 1) * 4;
    (&DAT_800b10ac)[iVar1] = 0;
    (&DAT_800b10ad)[iVar1] = 0;
    (&DAT_800b10ae)[iVar1] = 0;
  }
  return;
}
