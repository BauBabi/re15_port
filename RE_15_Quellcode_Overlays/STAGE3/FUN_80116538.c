/* FUN_80116538 @ 0x80116538  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116538(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012a0c(2000);
  func_0x80012974(4000);
  (**(code **)(&LAB_8011eea4 + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}


