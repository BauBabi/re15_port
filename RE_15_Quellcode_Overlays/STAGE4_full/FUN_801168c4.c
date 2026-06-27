/* FUN_801168c4 @ 0x801168c4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801168c4(void)

{
  if ((_DAT_800aca40 & 0x20000000) == 0) {
    (*(code *)(&PTR_FUN_8011a2c0)[*(byte *)(_DAT_800ac784 + 4)])();
  }
  return;
}


