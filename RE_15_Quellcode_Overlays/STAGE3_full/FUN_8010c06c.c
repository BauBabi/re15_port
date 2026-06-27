/* FUN_8010c06c @ 0x8010c06c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c06c(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1b14))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_80106b24();
  }
  else {
    FUN_8010698c();
  }
  return;
}


