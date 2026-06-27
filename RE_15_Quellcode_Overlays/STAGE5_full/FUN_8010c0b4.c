/* FUN_8010c0b4 @ 0x8010c0b4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c0b4(void)

{
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) == 0) {
    (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
                (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee0bd4))();
  }
  else if (*(char *)(_DAT_800ac784 + 5) == '\x02') {
    FUN_80106b6c();
  }
  else {
    FUN_801069d4();
  }
  return;
}


