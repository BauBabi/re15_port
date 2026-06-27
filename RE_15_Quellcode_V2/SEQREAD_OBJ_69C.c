void SEQREAD_OBJ_69C(void)

{
  int in_t0;
  short unaff_s1;
  short unaff_s4;
  int unaff_s5;
  
  SpuVmSetProgVol((int)*(short *)(unaff_s5 + 0x4c),*(undefined1 *)(unaff_s5 + in_t0 + 0x2c));
  SpuVmSetVol((int)unaff_s1 << 8 | (int)unaff_s4,(int)*(short *)(unaff_s5 + 0x4c),
              *(undefined1 *)(unaff_s5 + in_t0 + 0x2c),(int)*(short *)(in_t0 * 2 + unaff_s5 + 0x4e))
  ;
  SEQREAD_OBJ_804((int)unaff_s4,(int)unaff_s1);
  return;
}
