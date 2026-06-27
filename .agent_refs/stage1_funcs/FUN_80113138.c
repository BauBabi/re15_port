/* FUN_80113138 @ 0x80113138  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113138(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(7);
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 10;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  iVar1 = *(short *)(_DAT_800ac784 + 0x1ba) + -400;
  if (iVar1 <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = iVar1;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    FUN_80115d74(0);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 0;
  }
  return;
}


