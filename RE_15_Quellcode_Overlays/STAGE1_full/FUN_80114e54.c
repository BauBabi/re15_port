/* FUN_80114e54 @ 0x80114e54  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114e54(void)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  
  *(char *)(_DAT_800ac784 + 0x82) = -(char)(*(int *)(_DAT_800ac784 + 0x38) / 0x708);
  uVar1 = func_0x8001c6e8(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),8
                          ,0x400);
  *(undefined2 *)(_DAT_800ac784 + 0x1ba) = uVar1;
  iVar2 = _DAT_800aca88 - *(int *)(_DAT_800ac784 + 0x34);
  iVar3 = _DAT_800aca90 - *(int *)(_DAT_800ac784 + 0x3c);
  uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
  *(undefined2 *)(_DAT_800ac784 + 0x1dc) = uVar1;
  *(short *)(_DAT_800ac784 + 0x1ec) = _DAT_800aca8c - *(short *)(_DAT_800ac784 + 0x38);
  iVar2 = func_0x8004efe4(0x800b1028,0x1d);
  if ((iVar2 != 0) && (*(char *)(_DAT_800ac784 + 5) != '\x03')) {
    FUN_80115d74(3);
  }
  (*(code *)(&PTR_FUN_80121220)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


