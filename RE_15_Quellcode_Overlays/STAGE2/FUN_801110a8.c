/* FUN_801110a8 @ 0x801110a8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801110a8(void)

{
  byte bVar1;
  char cVar2;
  ushort uVar3;
  short sVar4;
  int iVar5;
  uint uVar6;
  int extraout_v1;
  undefined8 uVar7;
  
  if (*(byte *)(_DAT_800ac784 + 9) < 3) {
    bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
    if ((((bVar1 & 3) != 0) && (*(byte *)(_DAT_800ac784 + 9) < 2)) &&
       ((((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff) < 0x400))
    {
      *(ushort *)(_DAT_800ac784 + 0x1ec) = bVar1 & 1;
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = *(undefined1 *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
      FUN_80111c48();
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0;
    if (*(char *)(_DAT_800ac784 + 9) == '\x02') {
      *(undefined2 *)(_DAT_800ac784 + 0x1e2) = 0x800;
    }
    if (((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 -
         ((int)*(short *)(_DAT_800ac784 + 0x6a) + (int)*(short *)(_DAT_800ac784 + 0x1e2))) + 0x200 &
        0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined2 *)(_DAT_800ac784 + 4) = 0x101;
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
      uVar3 = (*(short **)(_DAT_800ac784 + 0x1b4))[2];
      uVar6 = (uint)uVar3;
      *(ushort *)(_DAT_800ac784 + 0x1ea) = **(short **)(_DAT_800ac784 + 0x1b4) + uVar3;
      goto LAB_80111c4c;
    }
  }
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar6 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x1f) * (ushort)*(byte *)(_DAT_800ac784 + 6) + 0x20
    ;
    if (*(byte *)(_DAT_800ac784 + 6) >> 1 == 0) {
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 0x50;
    }
  }
  else if (uVar6 != 1) {
LAB_80111c4c:
    *(undefined1 *)(*(int *)(uVar6 - 0x387c) + 0x94) = 0xb;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar3 & 0x1f) + 10;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar4 = func_0x8001a9cc(&DAT_800aca88,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar4;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    func_0x800245d8(0);
    sVar4 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar4 + -1;
    if ((sVar4 == 0) || (iVar5 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar5 != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    }
    if ((*(short *)(_DAT_800ac784 + 0x1d2) != 0) && (DAT_800acae7 == 0)) {
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
      uVar6 = func_0x8001af20();
      func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee7120 + (uVar6 & 1)),0x32);
      *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      uVar3 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar3 & 1) * -0x658 + 0x32c;
      iVar5 = _DAT_800ac784;
      func_0x8001af20();
      if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
        FUN_80112f50();
        return;
      }
      *(undefined2 *)(iVar5 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
      uVar3 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 200;
      cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
      iVar5 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
      uVar7 = func_0x800245d8();
      *(int *)(iVar5 + 0x38) = (int)((ulonglong)uVar7 >> 0x20) + (int)uVar7;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
        *(undefined1 *)(_DAT_800ac784 + 7) = 2;
      }
      return;
    }
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(byte *)(_DAT_800ac784 + 0x95) != 0) && (*(byte *)(_DAT_800ac784 + 0x95) < 9)) {
    FUN_80111bc4(1,(int)*(short *)(_DAT_800ac784 + 0x166));
    return;
  }
  FUN_8011388c(0,(int)*(short *)(_DAT_800ac784 + 0x166));
  *(ushort *)(_DAT_800ac784 + 0x8c) = (ushort)*(byte *)(_DAT_800ac784 + 0x8c);
  sVar4 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar4 + -1;
  if (sVar4 == 0) {
    *(undefined2 *)(_DAT_800ac784 + 4) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
    if (*(byte *)(_DAT_800ac784 + 9) - 3 < 2) {
      *(undefined2 *)(_DAT_800ac784 + 6) = 0xd;
    }
  }
  return;
}


