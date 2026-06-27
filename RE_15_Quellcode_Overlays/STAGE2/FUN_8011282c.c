/* FUN_8011282c @ 0x8011282c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011282c(void)

{
  short sVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar1;
  FUN_801118a8();
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -0x14;
  func_0x800245d8(0);
  return;
}


