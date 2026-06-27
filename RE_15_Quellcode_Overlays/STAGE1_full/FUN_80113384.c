/* FUN_80113384 @ 0x80113384  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113384(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 1) == 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
      uVar3 = 4;
    }
    else {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
      uVar3 = 3;
    }
    FUN_80115d94(uVar3);
    cVar1 = FUN_80115dc8();
    *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  if (*(byte *)(_DAT_800ac784 + 0x1d6) < 3) {
    func_0x8001a8f8(&DAT_800aca88,0x1e);
    FUN_80115e24();
  }
  else if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 4) != 0) {
    func_0x8001a8f8(&DAT_800aca88,0x1e);
  }
  func_0x800245d8(0);
  uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = uVar2;
  return;
}


