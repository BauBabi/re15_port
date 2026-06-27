void SEQREAD_OBJ_17FC(void)

{
  short unaff_s2;
  short unaff_s4;
  short unaff_s5;
  
  _SsUtBuildADSR(&stack0x0000005c,&stack0x0000004c,&stack0x0000004e);
  SsUtSetVagAtr(unaff_s2,unaff_s4,unaff_s5,(VagAtr *)&stack0x0000003c);
  SEQREAD_OBJ_1878();
  return;
}
