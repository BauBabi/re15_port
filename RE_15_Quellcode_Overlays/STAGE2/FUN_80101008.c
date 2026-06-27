/* FUN_80101008 @ 0x80101008  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101008(void)

{
  (**(code **)(&LAB_80117adc + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80117ad8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


