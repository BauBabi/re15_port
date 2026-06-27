/* FUN_80013b4c @ 0x80013b4c  (Ghidra headless, raw MIPS overlay) */

void FUN_80013b4c(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    uVar1 = DAT_800d5b50;
    if (-1 < (int)DAT_800d5b50) {
      do {
        FUN_80013ac8(uVar1);
        uVar1 = uVar1 - 1;
      } while (-1 < (int)uVar1);
    }
  }
  else {
    uVar1 = 0;
    do {
      FUN_80013ac8(uVar1);
      uVar1 = uVar1 + 1;
    } while (uVar1 <= DAT_800d5b50);
  }
  return;
}


