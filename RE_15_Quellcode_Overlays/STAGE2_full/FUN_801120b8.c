/* FUN_801120b8 @ 0x801120b8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801120b8(void)

{
  byte bVar1;
  char cVar2;
  undefined2 uVar3;
  ushort uVar4;
  short sVar5;
  int iVar6;
  uint uVar7;
  
  if (*(byte *)(_DAT_800ac784 + 9) < 2) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if (((bVar1 & 3) != 0) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(ushort *)(_DAT_800ac784 + 0x1ec) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 9;
      return;
    }
    if (((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) +
         0x200 & 0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 2) +
           *(short *)(*(int *)(_DAT_800ac784 + 0x1b4) + 6);
      if ((*(byte *)(_DAT_800ac784 + 0x90) & 0x40) == 0) {
        return;
      }
      *(undefined2 *)(_DAT_800ac784 + 0x1ee) = 1;
      *(undefined2 *)(_DAT_800ac784 + 0x1e8) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 4);
      *(short *)(_DAT_800ac784 + 0x1ea) =
           **(short **)(_DAT_800ac784 + 0x1b4) + (*(short **)(_DAT_800ac784 + 0x1b4))[2];
      return;
    }
  }
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    break;
  case 1:
  case 7:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x1f) + 0x14;
  case 3:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    uVar3 = func_0x8001a9cc(&DAT_800aca88,0x40);
    *(undefined2 *)(_DAT_800ac784 + 0x1e0) = uVar3;
    *(short *)(_DAT_800ac784 + 0x6a) =
         *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e0);
    if (*(short *)(_DAT_800ac784 + 0x1e0) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    }
    sVar5 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar5 + -1;
    if (sVar5 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar4 & 0x1f) + 10;
  case 5:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar5 = func_0x8001a9cc(&DAT_800aca88,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar5;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    func_0x800245d8(0);
    sVar5 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar5 + -1;
    if ((sVar5 == 0) || (iVar6 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar6 != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    }
    if (*(short *)(_DAT_800ac784 + 0x1d2) == 0) {
      return;
    }
    if (DAT_800acae7 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    func_0x800453d0(2);
    DAT_800aca58 = 2;
    cVar2 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar2 + 2;
    DAT_800aca5a = 0;
    DAT_800acae7 = DAT_800acae7 | 1;
    if (_DAT_800acaee < 0) {
      _DAT_800acaee = 1;
    }
    uVar7 = func_0x8001af20();
    func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee7120 + (uVar7 & 1)),0x32);
    return;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 7) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 7;
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  default:
    goto switchD_801122c4_default;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800245d8(0);
switchD_801122c4_default:
  return;
}


