void SEQREAD_OBJ_7F4(void)

{
  undefined4 uVar1;
  short unaff_s1;
  short unaff_s4;
  int unaff_s5;
  
  uVar1 = _SsReadDeltaValue((int)unaff_s4,(int)unaff_s1);
  *(undefined4 *)(unaff_s5 + 0x88) = uVar1;
  return;
}
