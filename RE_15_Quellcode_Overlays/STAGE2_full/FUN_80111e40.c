/* FUN_80111e40 @ 0x80111e40  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111e40(void)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  undefined2 uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(byte *)(_DAT_800ac784 + 6) = ((byte)*(undefined2 *)(_DAT_800ac784 + 0x1d0) & 1) * '\v';
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1bc) = _DAT_800aca88;
      *(undefined2 *)(_DAT_800ac784 + 0x1be) = _DAT_800aca90;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x1f) + 0x50;
  }
  uVar4 = func_0x8001a9cc(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e6));
  *(undefined2 *)(_DAT_800ac784 + 0x1e0) = uVar4;
  sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
  if ((sVar2 == 0) || (*(short *)(_DAT_800ac784 + 0x1e0) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e0);
  return;
}


