void SEQREAD_OBJ_1678(void)

{
  char unaff_s1;
  short unaff_s2;
  short unaff_s4;
  short unaff_s5;
  
  SsUtSetVagAtr(unaff_s2,unaff_s4,unaff_s5,(VagAtr *)&stack0x0000003c);
  if (unaff_s1 == '\0') {
    FUN_8005f87c();
    SEQREAD_OBJ_1878();
    return;
  }
  if ((((unaff_s1 != '\x01') && (unaff_s1 != '\x02')) && (unaff_s1 != '\x03')) &&
     (unaff_s1 == '\x04')) {
    SsUtReverbOn();
    SEQREAD_OBJ_1878();
    return;
  }
  return;
}
