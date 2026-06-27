void FUN_800170e0(int param_1)

{
  short sVar1;
  
  memcpy(&DAT_801fdd00,DAT_801a1004,param_1);
  DAT_800b2598 = &DAT_801fdd00 + *(int *)(&DAT_801fdcf8 + param_1);
  if (-1 < DAT_800b21ec) {
    SsVabClose((short)DAT_800b21ec);
    DAT_800b21ec = -1;
  }
  do {
    sVar1 = SsVabOpenHeadSticky(DAT_800b2598,0,0x1020);
    DAT_800b21ec = (char)sVar1;
  } while (DAT_800b21ec == -1);
  SsVabTransBody(DAT_801a1008,(short)DAT_800b21ec);
  SsVabTransCompleted(1);
  return;
}
