/* FUN_8010e570 @ 0x8010e570  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e570(void)

{
  if (*(char *)(_DAT_800ac784 + 0x1e0) == '\0') {
    (*(code *)(&PTR_FUN_80118c6c)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  else {
    FUN_8010e91c();
  }
  return;
}


