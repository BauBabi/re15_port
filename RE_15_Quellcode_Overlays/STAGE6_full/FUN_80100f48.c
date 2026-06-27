/* FUN_80100f48 @ 0x80100f48  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100f48(void)

{
  (**(code **)(&DAT_801025f4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_80102614 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


