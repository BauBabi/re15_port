void FUN_800134f0(byte param_1)

{
  DAT_800bed90 = param_1 >> 1;
  DAT_800bed91 = DAT_800bed90;
  DAT_800bed92 = DAT_800bed90;
  DAT_800bed93 = DAT_800bed90;
  CdMix((CdlATV *)&DAT_800bed90);
  return;
}
