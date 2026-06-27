/* FUN_8002aa28 @ 0x8002aa28  (Ghidra headless, raw MIPS overlay) */

void FUN_8002aa28(void)

{
  undefined4 uVar1;
  
  if ((DAT_800cfbd8 & 8) == 0) {
    uVar1 = 2;
    if ((DAT_800cfbd8 & 4) == 0) {
      return;
    }
  }
  else {
    uVar1 = 1;
  }
  FUN_8002aa6c(uVar1);
  return;
}


