/* CD_initintr @ 0x80088578  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void CD_initintr(void)

{
  DAT_800acdf0 = 0;
  DAT_800acdec = 0;
  DAT_800ace00 = 0;
  _DAT_800acdfc = 0;
  ResetCallback();
  InterruptCallback(2,callback);
  return;
}


