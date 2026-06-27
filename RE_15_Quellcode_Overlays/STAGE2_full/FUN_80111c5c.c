/* FUN_80111c5c @ 0x80111c5c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111c5c(void)

{
  short sVar1;
  ushort uVar2;
  int iVar3;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x9c) = uVar2 & 0x3f;
    *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0x18;
    if ((*(ushort *)(_DAT_800ac784 + 0x9c) & 1) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffe8;
    }
  }
  else if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    return;
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1e6);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0) ||
     (iVar3 = func_0x8001a9cc(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e6)), iVar3 != 0)) {
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
  }
  else {
    *(undefined1 *)(_DAT_800ac784 + 6) = 4;
  }
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  return;
}


