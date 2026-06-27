/* FUN_8010e91c @ 0x8010e91c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e91c(void)

{
  undefined2 uVar1;
  
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0);
  *(ushort *)(_DAT_800ac784 + 0x9c) = _DAT_800acabe - *(short *)(_DAT_800ac784 + 0x6a) & 0xfff;
  *(undefined1 *)(_DAT_800ac784 + 4) = 1;
  *(undefined1 *)(_DAT_800ac784 + 5) = 8;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  uVar1 = func_0x8001a9cc(&DAT_800aca88,0x20);
  *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar1;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = *(undefined2 *)(_DAT_800ac784 + 0x1da);
  return;
}


