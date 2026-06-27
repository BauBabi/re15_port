/* FUN_8010d160 @ 0x8010d160  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d160(void)

{
  int iVar1;
  uint uVar2;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(4000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  else {
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      return;
    }
    if (*(char *)(_DAT_800ac784 + 0x1e2) != '\0') {
      return;
    }
    if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
      return;
    }
    if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) {
      return;
    }
    iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20);
    if (iVar1 != 0) {
      return;
    }
    if ((_DAT_800aca58 != 0x701) && (uVar2 = func_0x8001af20(), (uVar2 & 1) == 0)) {
      return;
    }
    iVar1 = func_0x8001a780(&DAT_800aca54);
    if (iVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


