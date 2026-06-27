void SsUtSetReverbDelay(short param_1)

{
  DAT_800b2200 = (int)param_1;
  DAT_800b21f4 = 8;
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800b21f4);
  return;
}
