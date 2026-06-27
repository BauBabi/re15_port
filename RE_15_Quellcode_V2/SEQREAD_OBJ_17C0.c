void SEQREAD_OBJ_17C0(void)

{
  uint unaff_s1;
  short unaff_s2;
  short unaff_s4;
  short unaff_s5;
  undefined2 in_stack_0000006c;
  
  if (((unaff_s1 & 0xff) != 0) && ((unaff_s1 & 0xff) < 0x40)) {
    in_stack_0000006c = 0;
    SEQREAD_OBJ_1800();
    return;
  }
  if (0x3f < unaff_s1 - 0x40) {
    _SsUtBuildADSR(&stack0x0000005c,&stack0x0000004c,&stack0x0000004e);
    SsUtSetVagAtr(unaff_s2,unaff_s4,unaff_s5,(VagAtr *)&stack0x0000003c);
    SEQREAD_OBJ_1878();
    return;
  }
  in_stack_0000006c = 1;
  SEQREAD_OBJ_1800();
  return;
}
