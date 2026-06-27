void FUN_800449f4(byte param_1)

{
  DAT_800b5218 = 1;
  DAT_800b5360 = param_1;
  if (DAT_800b52ac != '\0') {
    SsSeqSetDecrescendo((short)DAT_800b52ae,0,(uint)param_1);
  }
  if (DAT_800b52b4 != '\0') {
    SsSeqSetDecrescendo((short)DAT_800b52b6,0,(uint)DAT_800b5360);
  }
  if (DAT_800b52bc != '\0') {
    SsSeqSetDecrescendo((short)DAT_800b52be,0,(uint)DAT_800b5360);
  }
  return;
}
