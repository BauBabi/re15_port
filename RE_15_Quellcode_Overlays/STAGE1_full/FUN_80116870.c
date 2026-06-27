/* FUN_80116870 @ 0x80116870  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116870(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x1d4);
    *(short *)(_DAT_800ac784 + 0x1d4) = sVar2 + -1;
    if (sVar2 != 0) {
      return;
    }
    if (0x2f < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      return;
    }
    if (7 < *(byte *)(_DAT_800ac784 + 0x1d0)) {
      FUN_80116bec();
    }
    *(char *)(_DAT_800ac784 + 0x1d0) = *(char *)(_DAT_800ac784 + 0x1d0) + '\x01';
    cVar3 = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (bVar1 < 2) {
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = 0x2c;
    FUN_80116d00();
    cVar3 = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else {
    if (bVar1 != 2) {
      return;
    }
    FUN_80116c68();
    cVar3 = '\x01';
  }
  *(char *)(_DAT_800ac784 + 6) = cVar3;
  return;
}


