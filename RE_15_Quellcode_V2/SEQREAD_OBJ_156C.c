void SEQREAD_OBJ_156C(int param_1)

{
  undefined4 uVar1;
  int unaff_s2;
  short in_stack_00000090;
  
  uVar1 = _SsReadDeltaValue(param_1 >> 0x10,(int)in_stack_00000090);
  *(undefined4 *)(unaff_s2 + 0x88) = uVar1;
  *(undefined1 *)(unaff_s2 + 0x2a) = 0;
  SEQREAD_OBJ_15A8();
  return;
}
