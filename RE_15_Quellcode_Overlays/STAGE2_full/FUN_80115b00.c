/* FUN_80115b00 @ 0x80115b00  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115b00(void)

{
  (**(code **)(&DAT_80118fe8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80119008 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


