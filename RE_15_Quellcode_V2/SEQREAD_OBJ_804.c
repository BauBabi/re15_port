void SEQREAD_OBJ_804(void)

{
  undefined4 uVar1;
  int unaff_s5;
  
  uVar1 = _SsReadDeltaValue();
  *(undefined4 *)(unaff_s5 + 0x88) = uVar1;
  return;
}
