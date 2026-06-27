/* FUN_80100a00 @ 0x80100a00  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100a00(void)

{
  (**(code **)(&DAT_80102564 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102584 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


