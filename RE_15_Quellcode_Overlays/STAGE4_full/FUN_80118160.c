/* FUN_80118160 @ 0x80118160  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118160(void)

{
  (**(code **)(&DAT_8011a564 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011a584 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


