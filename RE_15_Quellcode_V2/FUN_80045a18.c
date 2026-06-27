bool FUN_80045a18(uint param_1,byte param_2)

{
  bool bVar1;
  
  if ((param_2 & 7) < (byte)(&DAT_800b22cc)[param_1 & 0xff]) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
    if ((&DAT_800b22cc)[param_1 & 0xff] == (param_2 & 7)) {
      bVar1 = 7 < param_2;
    }
  }
  return bVar1;
}
