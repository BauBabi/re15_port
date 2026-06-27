/* FUN_8010c100 @ 0x8010c100  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c100(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1874))();
  }
  else {
    FUN_80107d9c();
  }
  return;
}


