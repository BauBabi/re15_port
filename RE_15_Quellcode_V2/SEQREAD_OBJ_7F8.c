void SEQREAD_OBJ_7F8(int param_1)

{
  undefined4 uVar1;
  short unaff_s1;
  int unaff_s5;
  
  uVar1 = _SsReadDeltaValue(param_1 >> 0x10,(int)unaff_s1);
  *(undefined4 *)(unaff_s5 + 0x88) = uVar1;
  return;
}
