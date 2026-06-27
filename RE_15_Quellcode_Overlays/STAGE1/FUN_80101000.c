/* FUN_80101000 @ 0x80101000  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101000(void)

{
  (**(code **)(&LAB_8011f9b0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_8011f920 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


