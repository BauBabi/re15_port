/* FUN_80112230 @ 0x80112230  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112230(int param_1)

{
  char cVar1;
  undefined1 uVar2;
  ushort uVar3;
  uint uVar4;
  int iVar5;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 == 1) {
LAB_801122b8:
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xffb0;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x3c;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_801122f4:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
      func_0x800453d0(1);
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    }
    *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    FUN_80115e24();
    func_0x800245d8(0);
    iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar5 != 0) {
      FUN_80115d74(5);
    }
    return;
  }
  if (uVar4 < 2) {
    if (uVar4 == 0) {
      func_0x800453d0(1);
      *(undefined1 *)(_DAT_800ac784 + 0x1da) = 1;
      FUN_80115d94(6);
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      goto LAB_801122b8;
    }
  }
  else if (uVar4 == 2) goto LAB_801122f4;
  if (param_1 == 0) {
    if ((*(byte *)(uVar4 + 0x1d4) & 1) != 0) {
      *(undefined2 *)(uVar4 + 0x8c) = 0xa0;
      _DAT_000000d8 = _DAT_00000284 + _DAT_000000d8;
      func_0x800245d8(0);
      return;
    }
    *(undefined2 *)(uVar4 + 0x8c) = 0xb4;
    FUN_80115d94(4);
    cVar1 = FUN_80115dc8();
    *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (param_1 != 1) goto LAB_801134e8;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 4) != 0) {
      func_0x8001a8f8(&DAT_800aca88,0x1e);
    }
    func_0x800245d8(0);
    uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar2;
    return;
  }
  func_0x8001a8f8(&DAT_800aca88,0x1e);
  uVar4 = FUN_80115e24();
  if ((uVar4 & 1) != 0) {
    FUN_80115d94(3);
    uVar3 = *(ushort *)(_DAT_800ac784 - 0x3512) - 4;
    *(ushort *)(_DAT_800ac784 - 0x3512) = uVar3;
    if ((int)((uint)uVar3 << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    return;
  }
  FUN_80115d94(4);
  uVar4 = _DAT_800ac784;
LAB_801134e8:
  *(undefined2 *)(uVar4 + 0x8c) = 0xb4;
  *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
  if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
    *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
  }
  uVar4 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  iVar5 = *(int *)(uVar4 - 0x387c);
  if ((*(ushort *)(iVar5 + 0x1dc) < 1000) &&
     (*(short *)(iVar5 + 0x1ba) + -0x2ee < *(int *)(iVar5 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}


