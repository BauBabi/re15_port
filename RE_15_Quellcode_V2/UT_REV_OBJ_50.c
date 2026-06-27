void UT_REV_OBJ_50(void)

{
  int in_v0;
  short in_v1;
  
  _DAT_800b21f8 = in_v0 >> 0x10;
  if (in_v1 == 0) {
    SpuSetReverb(0);
  }
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800b21f4);
  UT_REV_OBJ_90();
  return;
}
