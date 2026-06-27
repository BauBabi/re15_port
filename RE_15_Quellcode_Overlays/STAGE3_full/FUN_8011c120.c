/* FUN_8011c120 @ 0x8011c120  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c120(void)

{
  (**(code **)(&DAT_8011f5ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8011f60c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


