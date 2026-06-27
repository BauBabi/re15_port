/* FUN_80117a3c @ 0x80117a3c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117a3c(void)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  
  if (((DAT_800acae7 == '\0') && (iVar2 = func_0x8001a804(3000,0x180,&DAT_800aca88), iVar2 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    uVar1 = func_0x8001a9cc(&DAT_800aca88,0x20);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar1;
    if ((*(short *)(_DAT_800ac784 + 0x1da) != 0) && (*(short *)(_DAT_800ac784 + 0x1da) < 1)) {
      return;
    }
    uVar3 = func_0x8001a804(0x9c4,0x100,&DAT_800aca88);
    if ((uVar3 & 0x80000000) == 0) {
      return;
    }
    if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
      return;
    }
    if (*(char *)(_DAT_800ac784 + 0x1e1) != '\0') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
    return;
  }
  if (DAT_800acae7 != '\0') {
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0'))
    {
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      goto LAB_80117c68;
    }
    if (DAT_800acae7 != '\0') {
      return;
    }
  }
  if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) &&
      ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  if (DAT_800acae7 != '\0') {
    return;
  }
  if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) {
    return;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 4;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
LAB_80117c68:
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


