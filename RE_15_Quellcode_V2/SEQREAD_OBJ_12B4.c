void SEQREAD_OBJ_12B4(void)

{
  undefined4 uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int unaff_s2;
  int unaff_s7;
  short unaff_s8;
  byte in_stack_00000048;
  uchar in_stack_0000005c;
  short in_stack_00000090;
  
  iVar4 = 0;
  if (in_stack_00000048 != 0) {
    iVar3 = 0;
    do {
      sVar2 = (short)((uint)iVar3 >> 0x10);
      SsUtGetVagAtr(*(short *)(unaff_s2 + 0x4c),(ushort)*(byte *)(unaff_s7 + unaff_s2 + 0x2c),sVar2,
                    (VagAtr *)&stack0x00000058);
      SsUtSetVagAtr(*(short *)(unaff_s2 + 0x4c),(ushort)*(byte *)(unaff_s7 + unaff_s2 + 0x2c),sVar2,
                    (VagAtr *)&stack0x00000058);
      iVar4 = iVar4 + 1;
      iVar3 = iVar4 * 0x10000;
    } while (iVar4 < (int)(uint)in_stack_00000048);
  }
  uVar1 = _SsReadDeltaValue((int)unaff_s8,(int)in_stack_00000090);
  *(undefined4 *)(unaff_s2 + 0x88) = uVar1;
  *(undefined1 *)(unaff_s2 + 0x29) = 0;
  SEQREAD_OBJ_15A8();
  return;
}
