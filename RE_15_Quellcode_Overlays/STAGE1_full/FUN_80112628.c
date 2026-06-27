/* FUN_80112628 @ 0x80112628  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112628(void)

{
  bool bVar1;
  short sVar2;
  
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + -0x28;
    }
    else {
      sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + 0x28;
    }
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2;
    FUN_80115d74(*(byte *)(_DAT_800ac784 + 0x1d4) & 3);
  }
  if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
    bVar1 = *(ushort *)(_DAT_800ac784 + 0x1dc) < 5000;
  }
  else {
    bVar1 = *(ushort *)(_DAT_800ac784 + 0x1dc) < 10000;
  }
  if ((bVar1) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
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


