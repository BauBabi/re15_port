/* UT_REV_OBJ_50 @ 0x800811b4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void UT_REV_OBJ_50(void)

{
  int in_v0;
  short in_v1;
  
  _DAT_800d4c54 = in_v0 >> 0x10;
  if (in_v1 == 0) {
    SpuSetReverb(0);
  }
  SpuSetReverbModeParam((SpuReverbAttr *)&DAT_800d4c50);
  UT_REV_OBJ_90();
  return;
}


