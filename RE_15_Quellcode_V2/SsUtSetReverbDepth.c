void SsUtSetReverbDepth(short param_1,short param_2)

{
  DAT_800b21fc = (undefined2)((param_1 * 0x7fff) / 0x7f);
  DAT_800b21f4 = 6;
  DAT_800b21fe = (undefined2)((param_2 * 0x7fff) / 0x7f);
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800b21f4);
  return;
}
