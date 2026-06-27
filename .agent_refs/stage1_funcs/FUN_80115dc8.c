/* FUN_80115dc8 @ 0x80115dc8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_80115dc8(void)

{
  undefined4 uVar1;
  
  uVar1 = 1;
  if (*(short *)(_DAT_800ac784 + 0x1ea) + -0x1518 <= *(int *)(_DAT_800ac784 + 0x38)) {
    if (*(short *)(_DAT_800ac784 + 0x1ea) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      uVar1 = 0xffffffff;
    }
    else {
      uVar1 = 0xffffffff;
      if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) == 0) {
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}


