/* FUN_8010d8c4 @ 0x8010d8c4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d8c4(void)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((DAT_800acae7 != '\0') || (iVar1 = func_0x8001a804(0x9c4,0xc0,&DAT_800aca88), -1 < iVar1)) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) &&
       ((_DAT_800aca58 == 0x701 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      FUN_8010e244();
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) &&
       ((_DAT_800aca58 == 0x201 && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 3;
      FUN_8010e244();
      return;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1e6) == 0) &&
       (((0x50 < _DAT_800acaee && (7000 < *(short *)(_DAT_800ac784 + 0x1d4))) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)))) {
      uVar2 = func_0x8001af20();
      uVar3 = func_0x8001af20();
      if ((uVar2 & 1) * (uVar3 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 4;
        FUN_8010e244();
        return;
      }
    }
    if ((_DAT_800aca50 & 1) == 0) {
      if (((*(byte *)(_DAT_800ac784 + 0x90) & 3) != 0) &&
         ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a))
           + 0x200 & 0xfff) < 0x400)) {
        *(undefined2 *)(_DAT_800ac784 + 0x1ea) =
             *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10);
        *(ushort *)(_DAT_800ac784 + 0x1e8) = *(byte *)(_DAT_800ac784 + 0x90) & 1;
        *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
        *(undefined2 *)(_DAT_800ac784 + 6) = 0;
      }
      return;
    }
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 3;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  FUN_8010e2f0();
  return;
}


