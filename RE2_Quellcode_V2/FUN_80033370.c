/* FUN_80033370 @ 0x80033370  (Ghidra headless, raw MIPS overlay) */

void FUN_80033370(void)

{
  uint uVar1;
  
  uVar1 = (uint)DAT_800d767e;
  if (1 < uVar1) {
    do {
      FUN_800333c4(uVar1);
      uVar1 = uVar1 - 2;
    } while (1 < (int)uVar1);
  }
  FUN_800333c4(0);
  return;
}


