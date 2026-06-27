/* FUN_8010bad8 @ 0x8010bad8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bad8(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&LAB_8011ea8c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
    func_0x8002b498(_DAT_800ac784);
    uVar2 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar2;
    func_0x8002b544();
    uVar2 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar2;
    uVar1 = 1;
    if ((*(byte *)(_DAT_800ac784 + 4) < 2) && (uVar1 = 0x10, 1 < *(byte *)(_DAT_800ac784 + 5) - 1))
    {
      uVar1 = 0x12;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = uVar1;
    func_0x8001b4e4();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


