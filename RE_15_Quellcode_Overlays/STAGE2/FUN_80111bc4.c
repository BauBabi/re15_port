/* FUN_80111bc4 @ 0x80111bc4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111bc4(void)

{
  short sVar1;
  char cVar2;
  ushort uVar3;
  int iVar4;
  undefined8 uVar5;
  
  if (*(short *)(_DAT_800ac784 + 0x1e0) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 4;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  iVar4 = _DAT_800ac784;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    *(undefined1 *)(*(int *)(iVar4 + -0x387c) + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar3 & 1) * -0x658 + 0x32c;
    iVar4 = _DAT_800ac784;
    func_0x8001af20();
    if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
      FUN_80112f50();
      return;
    }
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
  return;
}


