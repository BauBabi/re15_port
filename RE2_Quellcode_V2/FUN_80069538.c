/* FUN_80069538 @ 0x80069538  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80069538(void)

{
  undefined4 uVar1;
  uint uVar2;
  
  uVar2 = (uint)DAT_800cfd4e;
  uVar1 = 4;
  if ((DAT_800cfe10 == 0) && (uVar1 = 0, uVar2 <= (uint)(int)DAT_800cfd5a >> 1)) {
    if (uVar2 < 0x29) {
      uVar1 = 2;
      if (uVar2 < 0x15) {
        uVar1 = 3;
      }
    }
    else {
      uVar1 = 1;
    }
  }
  return uVar1;
}


