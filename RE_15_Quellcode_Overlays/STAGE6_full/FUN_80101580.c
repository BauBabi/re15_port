/* FUN_80101580 @ 0x80101580  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101580(void)

{
  (**(code **)(&DAT_801026c8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801026e8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


