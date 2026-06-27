/* FUN_80114930 @ 0x80114930  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114930(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    FUN_80115d94(4);
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    FUN_801152cc();
    return;
  }
  if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    FUN_801152cc();
    return;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar1 != 0) {
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + -1;
  }
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x8001a8f8(&DAT_800aca88,0x32);
  func_0x800245d8(0);
  if (*(char *)(_DAT_800ac784 + 0x1d0) != '\0') {
    if (DAT_800aca58 != '\x05') {
      _DAT_800acbfc = _DAT_800ac784;
      _DAT_800acbcc = *(undefined4 *)(_DAT_800ac784 + 0x178);
      _DAT_800aca5a = 0;
      _DAT_800acbd0 = *(undefined4 *)(_DAT_800ac784 + 0x17c);
      DAT_800aca58 = '\x05';
      DAT_800aca59 = 1;
    }
    FUN_80115d74(2);
    func_0x8004ef90(0x800b1028,0x1c);
  }
  return;
}


