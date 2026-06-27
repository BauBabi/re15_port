/* FUN_8010b000 @ 0x8010b000  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b000(void)

{
  (**(code **)(&DAT_80120324 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&DAT_801202a8 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


