/* DE_12_OBJ_88 @ 0x8007bdf0  (Ghidra headless, raw MIPS overlay) */

void DE_12_OBJ_88(void)

{
  short unaff_s1;
  short unaff_s2;
  short unaff_s3;
  
  _SsUtBuildADSR(&stack0x00000010,&stack0x0000005c,&stack0x0000005e);
  SsUtSetVagAtr(unaff_s1,unaff_s2,unaff_s3,(VagAtr *)&stack0x0000004c);
  return;
}


