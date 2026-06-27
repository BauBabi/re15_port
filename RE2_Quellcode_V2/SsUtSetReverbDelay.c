/* SsUtSetReverbDelay @ 0x800806c4  (Ghidra headless, raw MIPS overlay) */

void SsUtSetReverbDelay(short param_1)

{
  DAT_800d4c5c = (int)param_1;
  DAT_800d4c50 = 8;
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800d4c50);
  return;
}


