/* FUN_8010e69c @ 0x8010e69c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e69c(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)(_DAT_800ac784 + 9) & 0x20) != 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee72b0))();
  }
  return;
}


