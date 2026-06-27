/* SsUtSetReverbFeedback @ 0x800812e8  (Ghidra headless, raw MIPS overlay) */

void SsUtSetReverbFeedback(short param_1)

{
  DAT_800d4c60 = (int)param_1;
  DAT_800d4c50 = 0x10;
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800d4c50);
  return;
}


