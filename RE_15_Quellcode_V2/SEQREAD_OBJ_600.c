void SEQREAD_OBJ_600(void)

{
  int in_t0;
  short unaff_s1;
  short unaff_s4;
  int unaff_s5;
  undefined2 unaff_s6;
  uint uStack00000010;
  
  uStack00000010 = (uint)*(byte *)(unaff_s5 + in_t0 + 0x17);
  SpuVmSetVol((int)unaff_s1 << 8 | (int)unaff_s4,(int)*(short *)(unaff_s5 + 0x4c),
              *(undefined1 *)(unaff_s5 + in_t0 + 0x2c));
  *(undefined2 *)(in_t0 * 2 + unaff_s5 + 0x4e) = unaff_s6;
  SEQREAD_OBJ_804((int)unaff_s4,(int)unaff_s1);
  return;
}
