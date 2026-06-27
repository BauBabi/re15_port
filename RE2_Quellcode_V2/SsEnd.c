/* SsEnd @ 0x8007a280  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void SsEnd(void)

{
  if ((DAT_800aba2c == 0) && (DAT_800aba39 = 0, DAT_800aba3a != '\x7f')) {
    FUN_800957a4();
    if (DAT_800aba38 != '\0') {
      VSyncCallback((f *)0x0);
      DAT_800aba38 = 0;
      SSEND_OBJ_A0();
      return;
    }
    if (DAT_800aba3a == '\0') {
      InterruptCallback(0,DAT_800aba34);
      DAT_800aba34 = 0;
      SSEND_OBJ_A0();
      return;
    }
    InterruptCallback(6,0);
    FUN_800957b4();
    DAT_800aba3a = '\x7f';
  }
  return;
}


