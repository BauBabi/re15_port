/* FUN_8005cf70 @ 0x8005cf70  (Ghidra headless, raw MIPS overlay) */

void FUN_8005cf70(void)

{
  DAT_800a7fac = DAT_800a7fac + 1;
  if (100 < DAT_800a7fac) {
    FUN_800957a4();
    DeliverEvent(0xf0000009,0x20);
    FUN_800957b4();
  }
  return;
}


