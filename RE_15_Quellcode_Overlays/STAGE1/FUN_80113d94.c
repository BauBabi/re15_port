/* FUN_80113d94 @ 0x80113d94  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113d94(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int unaff_s0;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 == 0) {
    FUN_80115d94(3);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (uVar4 != 1) {
    *(undefined2 *)(*(int *)(uVar4 - 0x387c) + 0x1ba) = 1;
    iVar2 = _DAT_800aca88 - *(int *)(_DAT_800ac784 + 0x34);
    iVar3 = _DAT_800aca90 - *(int *)(_DAT_800ac784 + 0x3c);
    uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = uVar1;
    *(short *)(_DAT_800ac784 + 0x1ec) = _DAT_800aca8c - *(short *)(_DAT_800ac784 + 0x38);
    iVar2 = func_0x8004efe4(unaff_s0 + 0x48a4,0x1d);
    if ((iVar2 != 0) && (*(char *)(_DAT_800ac784 + 5) != '\x03')) {
      FUN_80115d74(3);
    }
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedede0))();
    return;
  }
  func_0x8001a8f8(&DAT_800aca88,100);
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) + 0x30;
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee) {
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  }
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(ushort *)(_DAT_800ac784 + 0x1dc) < 1000) &&
     (*(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee < *(int *)(_DAT_800ac784 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}


