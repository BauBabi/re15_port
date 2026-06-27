/* FUN_80111de4 @ 0x80111de4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111de4(void)

{
  char cVar1;
  ushort uVar2;
  int in_v0;
  int extraout_v1;
  int iVar3;
  undefined8 uVar4;
  
  *(undefined1 *)(in_v0 + 7) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 9;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 7;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
  func_0x800245d8(0);
  *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1e6) = (uVar2 & 1) * -0x658 + 0x32c;
  iVar3 = _DAT_800ac784;
  func_0x8001af20();
  if ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) {
    FUN_80112f50();
    return;
  }
  *(undefined2 *)(iVar3 + 0x1e6) = *(undefined2 *)(_DAT_800ac784 + 0x1e6);
  uVar2 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar2 & 0x3f) + 200;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  iVar3 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
  uVar4 = func_0x800245d8();
  *(int *)(iVar3 + 0x38) = (int)((ulonglong)uVar4 >> 0x20) + (int)uVar4;
  if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}


