/* FUN_80111820 @ 0x80111820  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111820(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  uVar1 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x1d4) = uVar1;
  iVar3 = func_0x8004efe4(0x800b1028,0x1f);
  if (((iVar3 != 0) && (*(char *)(_DAT_800ac784 + 4) == '\x04')) &&
     ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 3;
    FUN_80115d74(0);
    func_0x8004efb8(0x800b1028,0x1f);
  }
  iVar3 = func_0x8004efe4(0x800b1028,0x1d);
  if (((iVar3 != 0) && (DAT_800aca58 == '\x05')) && (DAT_800aca5a < 3)) {
    DAT_800aca5a = 3;
  }
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)(_DAT_800ac784 + 9) & 0x20) != 0)) {
    *(char *)(_DAT_800ac784 + 0x82) = -(char)(*(int *)(_DAT_800ac784 + 0x38) / 0x708);
    uVar2 = func_0x8001c6e8(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,8,0x400);
    *(undefined2 *)(_DAT_800ac784 + 0x1ba) = uVar2;
    (**(code **)(&DAT_8012111c + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
    func_0x8002b498(_DAT_800ac784);
    uVar1 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    *(undefined1 *)(_DAT_800ac784 + 0x1d0) = uVar1;
    func_0x8002b544();
    uVar1 = func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6)
                            ,4);
    *(undefined1 *)(_DAT_800ac784 + 0x1d1) = uVar1;
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


