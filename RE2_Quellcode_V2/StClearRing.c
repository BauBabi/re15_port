/* StClearRing @ 0x80086bbc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void StClearRing(void)

{
  DAT_800ea234 = 0;
  DAT_800ea230 = 0;
  DAT_800ea22c = 0;
  DAT_800dbb90 = 0;
  init_ring_status(0,DAT_800eae9c);
  DAT_800d5268 = 0;
  DAT_800d4c64 = 0;
  DAT_800cbc24 = 0;
  return;
}


