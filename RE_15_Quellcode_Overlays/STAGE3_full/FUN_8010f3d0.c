/* FUN_8010f3d0 @ 0x8010f3d0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f3d0(void)

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
  }
  else {
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  }
  (*(code *)(&PTR_FUN_8011eb30)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


