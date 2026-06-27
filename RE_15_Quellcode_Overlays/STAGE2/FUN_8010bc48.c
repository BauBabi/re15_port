/* FUN_8010bc48 @ 0x8010bc48  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc48(void)

{
  undefined2 uVar1;
  
  func_0x800279c8(0x280010,0,&LAB_80100194,*(undefined4 *)(_DAT_800ac784 + 4));
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fee7438))();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined2 *)(_DAT_800ac784 + 0x1d2) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = uVar1;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


