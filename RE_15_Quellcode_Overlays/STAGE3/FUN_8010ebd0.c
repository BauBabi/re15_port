/* FUN_8010ebd0 @ 0x8010ebd0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ebd0(void)

{
  if (*(int *)(_DAT_800ac784 + 0x38) < (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(short *)(_DAT_800ac784 + 0x9c) =
           *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xc) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
    FUN_8010f47c();
    return;
  }
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  (**(code **)(&LAB_8011eb30 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


