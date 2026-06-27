/* DecDCTReset @ 0x80094870  (Ghidra headless, raw MIPS overlay) */

void DecDCTReset(int mode)

{
  if (mode == 0) {
    ResetCallback();
  }
  MDEC_reset(mode);
  return;
}


