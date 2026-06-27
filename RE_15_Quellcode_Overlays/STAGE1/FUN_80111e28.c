/* FUN_80111e28 @ 0x80111e28  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111e28(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 0) {
      FUN_80112684();
      return;
    }
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + -0x28;
    FUN_80115d74(*(byte *)(_DAT_800ac784 + 0x1d4) & 3);
  }
  if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
    FUN_801126d8();
    return;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1dc) < 10000) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518))
  {
    FUN_80115d74(4);
  }
  if (((_DAT_800aca52 & 1) != 0) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if ((_DAT_800aca50 & 0x1000) != 0) {
    FUN_80115d74((uint)*(byte *)(_DAT_800ac784 + 0x1d4) % 3 + 1);
  }
  return;
}


