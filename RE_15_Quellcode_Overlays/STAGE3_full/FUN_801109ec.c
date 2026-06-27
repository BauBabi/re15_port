/* FUN_801109ec @ 0x801109ec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801109ec(short param_1,short param_2)

{
  if (*(short *)(_DAT_800ac784 + 0x8c) < param_2) {
    *(short *)(_DAT_800ac784 + 0x8c) = param_1 + *(short *)(_DAT_800ac784 + 0x8c);
    if (param_2 < *(short *)(_DAT_800ac784 + 0x8c)) {
      *(short *)(_DAT_800ac784 + 0x8c) = param_2;
    }
  }
  return;
}


