void SEQREAD_OBJ_F2C(void)

{
  undefined4 uVar1;
  int unaff_s0;
  
  uVar1 = _SsReadDeltaValue();
  *(undefined4 *)(unaff_s0 + 0x88) = uVar1;
  return;
}
