/* FUN_8010c570 @ 0x8010c570  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c570(void)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) != 0) &&
     ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400)) {
    *(byte *)(_DAT_800ac784 + 0x9f) = bVar1;
    iVar3 = _DAT_800ac784;
    *(undefined1 *)(_DAT_800ac784 + 5) = 10;
    *(undefined1 *)(iVar3 + 0x1e1) = 0;
    return;
  }
  if (DAT_800acae7 != '\0') {
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 5000) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) &&
       (uVar2 = 1, *(char *)(_DAT_800ac784 + 0x1e2) == '\0')) goto LAB_8010c668;
    if (DAT_800acae7 != '\0') {
      return;
    }
  }
  iVar3 = func_0x8001a804(6000,0x180,&DAT_800aca88);
  if (-1 < iVar3) {
    return;
  }
  uVar2 = 3;
  if (*(short *)(_DAT_800ac784 + 0x1dc) != 0) {
    return;
  }
LAB_8010c668:
  *(undefined1 *)(_DAT_800ac784 + 5) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


