/* FUN_80117010 @ 0x80117010  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117010(void)

{
  (**(code **)(&DAT_8011a330 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a350 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


