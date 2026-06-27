/* FUN_80115700 @ 0x80115700  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115700(void)

{
  undefined4 uVar1;
  
  if (((*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ea) + -0x708) &&
      (uVar1 = 7, (int)*(short *)(_DAT_800ac784 + 0x1ba) == (int)*(short *)(_DAT_800ac784 + 0x1ea)))
     || (uVar1 = 5, *(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    FUN_80115d74(uVar1);
  }
  return;
}


