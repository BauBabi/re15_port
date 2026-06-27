void FUN_80043b80(char param_1)

{
  if (param_1 != '\0') {
    SsStart();
  }
  SsSetMVol(DAT_800b5274,DAT_800b5276);
  SsSetRVol(DAT_800b2840,DAT_800b2842);
  SsSetSerialAttr('\0','\0','\0');
  SsSetSerialVol('\0',DAT_800b2244,DAT_800b2246);
  return;
}
