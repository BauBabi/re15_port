/* FUN_801101e4 @ 0x801101e4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801101e4(void)

{
  undefined2 uVar1;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)(byte)_DAT_800ac784[1] * 4 + -0x7fee71dc))();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)((int)_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0xd,*(undefined2 *)(_DAT_800ac784[0x1e] + 6),4);
    *(undefined2 *)((int)_DAT_800ac784 + 0x1da) = uVar1;
  }
  if ((*_DAT_800ac784 & 2) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x2d) = *(undefined2 *)(_DAT_800ac784[0x1e] + 6);
    *(short *)((int)_DAT_800ac784 + 0xb6) = (short)_DAT_800ac784[0x2d] + 100;
  }
  if ((int)_DAT_800ac784[0xe] < *(short *)((int)_DAT_800ac784 + 0x1ba) + -3000) {
    *(undefined2 *)(_DAT_800ac784 + 0x2d) = 0;
    *(undefined2 *)((int)_DAT_800ac784 + 0xb6) = 0;
  }
  func_0x8001b064(_DAT_800ac784 + 0x2c,(int)*(short *)((int)_DAT_800ac784 + 0x1ba));
  return;
}


