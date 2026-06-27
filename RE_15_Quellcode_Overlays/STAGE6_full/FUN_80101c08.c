/* FUN_80101c08 @ 0x80101c08  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101c08(void)

{
  (**(code **)(&DAT_801027ec + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_8010280c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


