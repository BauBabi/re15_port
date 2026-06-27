/* FUN_80115e24 @ 0x80115e24  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115e24(void)

{
  byte bVar1;
  short sVar2;
  
  if (*(ushort *)(_DAT_800ac784 + 0x1dc) < 2000) {
    if (*(char *)(_DAT_800ac784 + 0x1d7) != '\0') goto LAB_80115ea4;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7) | 1;
    }
    else {
      if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) != 1) goto LAB_80115ea4;
      bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7) | 2;
    }
  }
  else {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7) & 0xfc;
  }
  *(byte *)(_DAT_800ac784 + 0x1d7) = bVar1;
LAB_80115ea4:
  bVar1 = *(byte *)(_DAT_800ac784 + 0x1d7);
  if (bVar1 == 1) {
    sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + 0x50;
  }
  else {
    if (bVar1 < 2) {
      return;
    }
    if (bVar1 != 2) {
      return;
    }
    sVar2 = *(short *)(_DAT_800ac784 + 0x6a) + -0x50;
  }
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2;
  return;
}


