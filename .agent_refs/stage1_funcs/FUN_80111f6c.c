/* FUN_80111f6c @ 0x80111f6c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f6c(short param_1,short param_2)

{
  if (param_2 < *(short *)(_DAT_800ac784 + 0x8c)) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) - param_1;
    if (*(short *)(_DAT_800ac784 + 0x8c) < param_2) {
      *(short *)(_DAT_800ac784 + 0x8c) = param_2;
    }
  }
  return;
}


