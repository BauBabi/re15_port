/* FUN_80116a04 @ 0x80116a04  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116a04(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
    FUN_80116c68();
  }
  else {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x10001;
      *(byte *)(_DAT_800ac784 + 5) = *(byte *)(_DAT_800ac784 + 9) & 0x7f;
      *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 3;
    if (((*(byte *)(_DAT_800ac784 + 0x1d0) < 8) ||
        (FUN_80116b70(), *(byte *)(_DAT_800ac784 + 0x1d0) < 8)) &&
       (bVar1 = *(byte *)(_DAT_800ac784 + 9) & 0x7f, bVar1 < 3)) {
      func_0x8004ef90(0x800b1028,bVar1 + 0x1d);
    }
    if (1 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + -1;
    }
  }
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + '\x01';
  return;
}


