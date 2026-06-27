/* SsUtSetReverbDepth @ 0x800810cc  (Ghidra headless, raw MIPS overlay) */

void SsUtSetReverbDepth(short param_1,short param_2)

{
  DAT_800d4c50 = 6;
  DAT_800d4c58 = (undefined2)((param_1 * 0x7fff) / 0x7f);
  DAT_800d4c5a = (undefined2)((param_2 * 0x7fff) / 0x7f);
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800d4c50);
  return;
}


