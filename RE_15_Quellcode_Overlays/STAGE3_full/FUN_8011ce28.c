/* FUN_8011ce28 @ 0x8011ce28  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ce28(void)

{
  (**(code **)(&DAT_8011f7e4 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}


