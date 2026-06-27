/* FUN_8011340c @ 0x8011340c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011340c(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1db) == '\0') {
    FUN_80115d74(0xe);
  }
  if (0x1518 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 4;
  }
  return;
}


