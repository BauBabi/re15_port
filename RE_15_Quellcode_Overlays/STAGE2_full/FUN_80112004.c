/* FUN_80112004 @ 0x80112004  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112004(void)

{
  short sVar1;
  
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x30);
  *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar1;
  FUN_801118a8();
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 100;
  func_0x800245d8(0);
  return;
}


