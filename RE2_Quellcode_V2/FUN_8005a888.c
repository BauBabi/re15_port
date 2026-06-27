/* FUN_8005a888 @ 0x8005a888  (Ghidra headless, raw MIPS overlay) */

void FUN_8005a888(void)

{
  if ((DAT_800cfb74 & 0x2000) == 0) {
    if (((DAT_800d75c1 != -1) && (DAT_800d75c1 == '\0')) && (-1 < DAT_800d75c2)) {
      SsSeqReplay((short)DAT_800d75c2);
      DAT_800d75c0 = 1;
    }
    if (((DAT_800d75c9 != -1) && (DAT_800d75c9 == '\0')) && (-1 < DAT_800d75ca)) {
      SsSeqReplay((short)DAT_800d75ca);
      DAT_800d75c8 = 1;
    }
    if (((DAT_800d75d1 != -1) && (DAT_800d75d1 == '\0')) && (-1 < DAT_800d75d2)) {
      SsSeqReplay((short)DAT_800d75d2);
      DAT_800d75d0 = 1;
    }
  }
  return;
}


