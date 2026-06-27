/* FUN_8010ee9c @ 0x8010ee9c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee9c(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)(_DAT_800ac784 + 9) & 0x20) != 0)) {
    (*(code *)(&PTR_FUN_80118d50)[*(byte *)(_DAT_800ac784 + 4)])();
  }
  return;
}


