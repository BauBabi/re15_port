/* FUN_8010c8f8 @ 0x8010c8f8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c8f8(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf660))();
  if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


