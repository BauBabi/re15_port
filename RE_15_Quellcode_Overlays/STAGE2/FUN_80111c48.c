/* FUN_80111c48 @ 0x80111c48  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c48(void)

{
  char cVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  int extraout_v1;
  undefined8 uVar6;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x9c) = (uVar2 & 0x1f) + 10;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar3 = func_0x8001a9cc(&DAT_800aca88,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar3;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
  func_0x800245d8(0);
  sVar3 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar3 + -1;
  if ((sVar3 == 0) || (iVar4 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar4 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
  }
  if ((*(short *)(_DAT_800ac784 + 0x1d2) != 0) && (DAT_800acae7 == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    func_0x800453d0(2);
    DAT_800aca58 = 2;
    cVar1 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar1 + 2;
    DAT_800aca5a = 0;
    DAT_800acae7 = DAT_800acae7 | 1;
    if (_DAT_800acaee < 0) {
      _DAT_800acaee = 1;
    }
    uVar5 = func_0x8001af20();
    func_0x80037edc(*(undefined1 *)((uint)DAT_800aca59 * 2 + -0x7fee7120 + (uVar5 & 1)),0x32);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar2 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      FUN_80112f50();
      return;
    }
    *(undefined2 *)(iVar4 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 200;
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
    iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
    uVar6 = func_0x800245d8();
    *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar6 >> 0x20) + (int)uVar6;
    if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 1;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    return;
  }
  return;
}


