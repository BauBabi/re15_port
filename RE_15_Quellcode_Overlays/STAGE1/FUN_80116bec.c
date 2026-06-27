/* FUN_80116bec @ 0x80116bec  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116bec(void)

{
  ushort uVar1;
  
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    *(short *)(_DAT_800ac784 + 0x1dc) = *(short *)(_DAT_800ac784 + 0x1dc) + -1;
  }
  if (*(char *)(_DAT_800ac784 + 0x1e1) != '\0') {
    *(char *)(_DAT_800ac784 + 0x1e1) = *(char *)(_DAT_800ac784 + 0x1e1) + -1;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
    uVar1 = *(ushort *)(_DAT_800ac784 + 0x1de);
    *(ushort *)(_DAT_800ac784 + 0x1de) = uVar1 + 1;
    *(undefined1 *)(uVar1 + 8) = 0;
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0;
  return;
}


