/* FUN_8007cbe8 @ 0x8007cbe8  (Ghidra headless, raw MIPS overlay) */

/* Possible S_Q.OBJ/SpuQuit */

void FUN_8007cbe8(void)

{
  if (DAT_800ab6e8 == 1) {
    DAT_800ab6e8 = 0;
    FUN_800957a4();
    DAT_800ab254 = 0;
    DAT_800ab258 = 0;
    _SpuDataCallback(0);
    CloseEvent(DAT_800ab280);
    DisableEvent(DAT_800ab280);
    FUN_800957b4();
  }
  return;
}


