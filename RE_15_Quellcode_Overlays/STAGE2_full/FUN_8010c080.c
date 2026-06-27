/* FUN_8010c080 @ 0x8010c080  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c080(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (*(code *)(&PTR_LAB_80118a20)[*(byte *)(_DAT_800ac784 + 4)])();
  }
  return;
}


