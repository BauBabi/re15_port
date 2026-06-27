/* FUN_80112658 @ 0x80112658  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112658(void)

{
  char cVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  
  uVar3 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar3 != 1) {
    iVar4 = _DAT_800ac784;
    if (1 < uVar3) {
      uVar5 = CONCAT44(uVar3,2);
      if (uVar3 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        *(undefined1 *)(_DAT_800ac784 + 5) = 0;
        *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0;
        return;
      }
      goto LAB_80113018;
    }
    uVar5 = CONCAT44(uVar3,7);
    if (uVar3 != 0) goto LAB_80113018;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
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
  }
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  iVar4 = (int)*(short *)(_DAT_800ac784 + 0x1e6);
  uVar5 = func_0x800245d8();
LAB_80113018:
  *(int *)(iVar4 + 0x38) = (int)((ulonglong)uVar5 >> 0x20) + (int)uVar5;
  if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x708 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xc;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  return;
}


