void SsUtSetReverbFeedback(short param_1)

{
  DAT_800b2204 = (int)param_1;
  DAT_800b21f4 = 0x10;
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800b21f4);
  return;
}
