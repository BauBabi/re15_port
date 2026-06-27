/* FUN_80112f6c @ 0x80112f6c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f6c(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 0x1d1) != '\0') {
    FUN_80115d74(0xe);
  }
  iVar1 = func_0x8001b9b4(&DAT_800aca88);
  if (iVar1 != 0) {
    FUN_80115d74(0xe);
  }
  if (0x1518 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(5);
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 4;
  }
  return;
}


