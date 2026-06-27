/* FUN_8011a5f8 @ 0x8011a5f8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011a5f8(void)

{
  *(undefined1 *)(_DAT_800ac784 + 5) = 2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
  func_0x800453d0();
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
  return;
}


