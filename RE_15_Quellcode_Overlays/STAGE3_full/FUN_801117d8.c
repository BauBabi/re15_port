/* FUN_801117d8 @ 0x801117d8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801117d8(void)

{
  undefined1 uVar1;
  int iVar2;
  
  if (((DAT_800acae7 != '\0') || (iVar2 = func_0x8001a804(3000,0x180,&DAT_800aca88), -1 < iVar2)) ||
     (uVar1 = 5, *(short *)(_DAT_800ac784 + 0x1dc) != 0)) {
    if (DAT_800acae7 != '\0') {
      return;
    }
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0'))
    {
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    if (DAT_800acae7 != '\0') {
      return;
    }
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) {
      return;
    }
    uVar1 = 4;
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar1;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


