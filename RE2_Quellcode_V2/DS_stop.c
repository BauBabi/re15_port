/* DS_stop @ 0x8008bcbc  (Ghidra headless, raw MIPS overlay) */

void DS_stop(void)

{
  DAT_800ad238 = 0;
  CD_flush();
  if ((DAT_800ad254 == 2) && (DAT_800ad258 == 0xb)) {
    DAT_800ad254 = 1;
  }
  return;
}


