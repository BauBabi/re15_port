void SEQREAD_OBJ_B98(void)

{
  undefined4 uVar1;
  int unaff_s0;
  int unaff_s1;
  int unaff_s2;
  int unaff_s3;
  char unaff_s4;
  int unaff_s5;
  short unaff_s6;
  short unaff_s7;
  byte in_stack_00000010;
  undefined4 in_stack_00000020;
  
  while( true ) {
    unaff_s1 = unaff_s1 + 1;
    SsUtSetVagAtr(*(short *)(unaff_s2 + 0x4c),(ushort)*(byte *)(unaff_s3 + 0x2c),
                  (short)((uint)unaff_s0 >> 0x10),(VagAtr *)&stack0x00000020);
    unaff_s0 = unaff_s0 + 0x10000;
    if ((int)(uint)in_stack_00000010 <= unaff_s1) {
      uVar1 = _SsReadDeltaValue((int)unaff_s6,(int)unaff_s7);
      *(undefined4 *)(unaff_s2 + 0x88) = uVar1;
      return;
    }
    SsUtGetVagAtr(*(short *)(unaff_s2 + 0x4c),(ushort)*(byte *)(unaff_s3 + 0x2c),
                  (short)((uint)unaff_s0 >> 0x10),(VagAtr *)&stack0x00000020);
    if (unaff_s5 != 0) break;
    if ((byte)(unaff_s4 - 0x40U) < 0x40) {
      in_stack_00000020._1_1_ = '\0';
    }
  }
  in_stack_00000020._1_1_ = '\x02';
  SEQREAD_OBJ_B98();
  return;
}
