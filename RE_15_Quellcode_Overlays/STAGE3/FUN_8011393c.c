/* FUN_8011393c @ 0x8011393c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011393c(void)

{
  *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
  }
  if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
  }
  return;
}


