/* FUN_80105920 @ 0x80105920  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105920(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee836c))();
    if ((*(short *)(_DAT_800ac784 + 0x1dc) < 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0x11;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  else if (*(byte *)(_DAT_800ac784 + 5) - 0x12 < 2) {
    FUN_801068cc();
  }
  else {
    FUN_80106734();
  }
  return;
}


