/* FUN_80111240 @ 0x80111240  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111240(void)

{
  undefined2 uVar1;
  ushort uVar2;
  uint uVar3;
  
  uVar1 = func_0x8001a9cc(&DAT_800aca88,0x80);
  *(undefined2 *)(_DAT_800ac784 + 0x1e2) = uVar1;
  uVar1 = func_0x8001a9cc(&DAT_800aca88,0x100);
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = uVar1;
  if (((_DAT_800aca58 == 0x701) && (*(short *)(_DAT_800ac784 + 0x1e2) != 0)) &&
     (DAT_800aca5a == '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 3;
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 8000) && (*(short *)(_DAT_800ac784 + 0x1e2) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 3;
  }
  if (*(short *)(_DAT_800ac784 + 0x1d6) == 0) {
    if ((*(short *)(_DAT_800ac784 + 0x1d4) < 0x1d4c) &&
       ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      uVar2 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 6) = (uVar2 & 1) + 6;
    }
    if (((*(short *)(_DAT_800ac784 + 0x1d4) < 6000) && (*(short *)(_DAT_800ac784 + 0x1e2) == 0)) &&
       (DAT_800acad6 <= *(byte *)(_DAT_800ac784 + 0x82))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 7;
    }
  }
  if (((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) &&
     (uVar3 = func_0x8001af20(), (uVar3 & 0xf) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  if (0x1e < *(short *)(_DAT_800ac784 + 0x1dc)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 10;
  }
  return;
}


