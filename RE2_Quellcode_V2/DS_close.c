/* DS_close @ 0x8008b014  (Ghidra headless, raw MIPS overlay) */

void DS_close(void)

{
  DAT_800ad238 = 0;
  CD_flush();
  DAT_800acdec = 0;
  DAT_800acdf0 = 0;
  VSyncCallbacks(0,0);
  return;
}


