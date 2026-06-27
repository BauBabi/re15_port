/* FUN_80100e40 @ 0x80100e40  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100e40(void)

{
  (**(code **)(&LAB_80118d68 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118db8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


