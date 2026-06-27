/* FUN_8011e00c @ 0x8011e00c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011e00c(void)

{
  (**(code **)(&DAT_80121820 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80121840 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


