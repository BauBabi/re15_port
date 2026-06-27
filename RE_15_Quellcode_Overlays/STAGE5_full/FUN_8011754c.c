/* FUN_8011754c @ 0x8011754c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011754c(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  func_0x80012a0c(2000);
  func_0x80012974(4000);
  (*(code *)(&PTR_FUN_8011fe68)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}


