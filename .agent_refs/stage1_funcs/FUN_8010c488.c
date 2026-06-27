/* FUN_8010c488 @ 0x8010c488  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c488(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  (*(code *)(&PTR_FUN_8012095c)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}


