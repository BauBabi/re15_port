void SEQREAD_OBJ_171C(void)

{
  uint unaff_s1;
  short unaff_s2;
  int unaff_s3;
  short unaff_s4;
  short unaff_s5;
  undefined4 in_stack_00000044;
  ushort uStack0000004c;
  ushort uStack0000004e;
  undefined2 uStack0000005c;
  undefined2 uStack0000005e;
  undefined2 in_stack_00000060;
  undefined2 uStack00000062;
  undefined2 in_stack_00000064;
  undefined2 uStack00000066;
  undefined2 in_stack_00000068;
  undefined2 uStack0000006a;
  undefined2 in_stack_0000006c;
  
  _SsUtResolveADSR(uStack0000004c,uStack0000004e,&stack0x0000005c);
  uStack00000062 = (undefined2)unaff_s1;
  switch((unaff_s3 + -4) * 0x10000 >> 0x10) {
  case 0:
    uStack00000066 = 0;
    uStack0000005c = uStack00000062;
    SEQREAD_OBJ_1800();
    return;
  case 1:
    uStack00000066 = 1;
    uStack0000005c = uStack00000062;
    SEQREAD_OBJ_1800();
    return;
  case 2:
    uStack0000005e = uStack00000062;
    break;
  case 3:
    in_stack_00000060 = uStack00000062;
    break;
  case 4:
    in_stack_00000068 = 0;
    SEQREAD_OBJ_1800();
    return;
  case 5:
    in_stack_00000068 = 1;
    SEQREAD_OBJ_1800();
    return;
  case 6:
    uStack0000006a = 0;
    in_stack_00000064 = uStack00000062;
    SEQREAD_OBJ_1800();
    return;
  case 7:
    uStack0000006a = 1;
    in_stack_00000064 = uStack00000062;
    SEQREAD_OBJ_1800();
    return;
  case 8:
    if (((unaff_s1 & 0xff) != 0) && ((unaff_s1 & 0xff) < 0x40)) {
      in_stack_0000006c = 0;
      SEQREAD_OBJ_1800();
      return;
    }
    if (unaff_s1 - 0x40 < 0x40) {
      in_stack_0000006c = 1;
      SEQREAD_OBJ_1800();
      return;
    }
    break;
  case 9:
    in_stack_00000044._1_1_ = (uchar)unaff_s1;
    break;
  case 10:
    in_stack_00000044._2_1_ = (uchar)unaff_s1;
  }
  _SsUtBuildADSR(&stack0x0000005c,&stack0x0000004c,(int)&stack0x0000004c + 2);
  SsUtSetVagAtr(unaff_s2,unaff_s4,unaff_s5,(VagAtr *)&stack0x0000003c);
  SEQREAD_OBJ_1878();
  return;
}
