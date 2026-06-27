/* FUN_8010dd70 @ 0x8010dd70  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010dd70(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1e0) != '\0') {
    FUN_8010e91c();
    FUN_8010e5c8();
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fee7394))();
  return;
}


