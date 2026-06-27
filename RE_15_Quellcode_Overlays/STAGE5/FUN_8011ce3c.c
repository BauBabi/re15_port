/* FUN_8011ce3c @ 0x8011ce3c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011ce3c(void)

{
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedf858))();
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 1) != 0) {
    func_0x800369f8(0,0);
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 2) != 0) {
    func_0x800369f8(0,1);
  }
  return;
}


