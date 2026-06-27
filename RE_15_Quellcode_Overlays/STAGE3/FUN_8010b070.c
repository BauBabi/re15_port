/* FUN_8010b070 @ 0x8010b070  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b070(void)

{
  (**(code **)(&DAT_8011e43c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (*(code *)(&PTR_LAB_8011e458)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


