/* FUN_801110d0 @ 0x801110d0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801110d0(void)

{
  undefined2 uVar1;
  undefined4 uVar2;
  
  uVar2 = 4;
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee5f4c))();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    if (*(char *)(_DAT_800ac784 + 9) == '\x10') {
      uVar2 = 8;
    }
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,uVar2);
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar1;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0xb4) = 1000;
    *(short *)(_DAT_800ac784 + 0xb6) = *(short *)(_DAT_800ac784 + 0xb4) + 100;
    if (*(int *)(_DAT_800ac784 + 0x38) < (int)*(short *)(_DAT_800ac784 + 0x1ba)) {
      *(undefined2 *)(_DAT_800ac784 + 0xb4) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0xb6) = 0;
    }
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


