undefined4 FUN_8004ed6c(void)

{
  undefined4 uVar1;
  
  uVar1 = 3;
  if ((DAT_800acaec & 2) == 0) {
    if (DAT_800acaee < 0x50) {
      uVar1 = 1;
      if (DAT_800acaee < 0x14) {
        uVar1 = 2;
      }
    }
    else {
      uVar1 = 0;
    }
  }
  return uVar1;
}
