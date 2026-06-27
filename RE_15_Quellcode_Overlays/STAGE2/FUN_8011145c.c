/* FUN_8011145c @ 0x8011145c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011145c(void)

{
  short sVar1;
  char cVar2;
  ushort uVar3;
  int extraout_v1;
  int iVar4;
  undefined8 uVar5;
  
  cVar2 = *(char *)(_DAT_800ac784 + 7);
  if (cVar2 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = uVar3 & 0x3f;
    *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x18;
    if ((*(ushort *)(_DAT_800ac784 + 0x9c) & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffe8;
    }
  }
  else if (cVar2 != '\x01') {
    DAT_00000096 = 0;
    *(char *)(_DAT_800ac784 + 0x8f) = cVar2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 7;
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    func_0x800245d8(0);
    *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar3 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) {
      *(undefined2 *)(iVar4 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
      uVar3 = func_0x8001af20();
      *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 0x3f) + 200;
      cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
      iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
      uVar5 = func_0x800245d8();
      *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar5 >> 0x20) + (int)uVar5;
      if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
        *(undefined1 *)(_DAT_800ac784 + 7) = 2;
      }
      return;
    }
    FUN_80112f50();
    return;
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e6);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) &&
     (iVar4 = func_0x8001a9cc(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e6)), iVar4 == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
    FUN_80111de4();
    return;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


