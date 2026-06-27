/* FUN_8010bf34 @ 0x8010bf34  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf34(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee673c))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_801069ec();
  }
  else {
    FUN_80106854();
  }
  return;
}


