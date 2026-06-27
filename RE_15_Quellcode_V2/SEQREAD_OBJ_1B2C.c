void SEQREAD_OBJ_1B2C(void)

{
  undefined4 uVar1;
  short in_t0;
  short in_t1;
  int unaff_s0;
  
  uVar1 = _SsReadDeltaValue((int)in_t0,(int)in_t1);
  *(undefined4 *)(unaff_s0 + 0x88) = uVar1;
  SEQREAD_OBJ_1C84();
  return;
}
