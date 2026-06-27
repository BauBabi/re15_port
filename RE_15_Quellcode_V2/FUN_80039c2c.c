void FUN_80039c2c(void)

{
  int iVar1;
  
  DAT_8008f6a4 = DAT_800ac77c;
  iVar1 = FUN_80013b60(0x29,DAT_800ac77c,1,"SHITAI");
  if (0 < iVar1) {
    iVar1 = FUN_80022150(2,DAT_8008f6a4,0xf,8);
    DAT_800ac77c = iVar1 + DAT_800ac77c;
  }
  return;
}
