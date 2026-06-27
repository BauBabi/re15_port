/* FUN_80113c0c @ 0x80113c0c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c0c(void)

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


