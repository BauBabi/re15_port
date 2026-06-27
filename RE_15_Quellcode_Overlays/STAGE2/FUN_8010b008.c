/* FUN_8010b008 @ 0x8010b008  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b008(void)

{
  (**(code **)(&LAB_80118480 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_80118484 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


