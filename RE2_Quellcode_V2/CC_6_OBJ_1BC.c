/* CC_6_OBJ_1BC @ 0x8007aa50  (Ghidra headless, raw MIPS overlay) */

void CC_6_OBJ_1BC(void)

{
  byte bVar1;
  undefined4 uVar2;
  int unaff_s0;
  int unaff_s1;
  int unaff_s2;
  byte unaff_s4;
  int unaff_s5;
  int unaff_s6;
  short unaff_s7;
  short unaff_s8;
  byte in_stack_00000038;
  uchar in_stack_0000004c;
  byte in_stack_00000054;
  byte bStack00000055;
  
  do {
    unaff_s1 = unaff_s1 + 1;
    SsUtSetVagAtr((ushort)*(byte *)(unaff_s0 + 0x26),(ushort)*(byte *)(unaff_s6 + 0x37),
                  (short)((uint)unaff_s2 >> 0x10),(VagAtr *)&stack0x00000048);
    unaff_s2 = unaff_s2 + 0x10000;
    if ((int)(uint)in_stack_00000038 <= unaff_s1) {
      uVar2 = _SsReadDeltaValue((int)unaff_s7,(int)unaff_s8);
      *(undefined4 *)(unaff_s0 + 0x90) = uVar2;
      *(undefined1 *)(unaff_s0 + 0x1e) = 0;
      CC_6_OBJ_3F4();
      return;
    }
    SsUtGetVagAtr((ushort)*(byte *)(unaff_s0 + 0x26),(ushort)*(byte *)(unaff_s6 + 0x37),
                  (short)((uint)unaff_s2 >> 0x10),(VagAtr *)&stack0x00000048);
    bVar1 = *(byte *)(unaff_s0 + 0x18);
    if (bVar1 == 1) {
      if (((byte)(unaff_s4 + 0xbf) < 0x3f) && (unaff_s5 << 2 < 0)) {
        CC_6_OBJ_1A4();
        return;
      }
      CC_6_OBJ_1BC();
      return;
    }
    if (bVar1 < 2) {
      bStack00000055 = unaff_s4 & 0x7f;
      if (bVar1 == 0) {
        in_stack_00000054 = bStack00000055;
        CC_6_OBJ_1BC();
        return;
      }
      CC_6_OBJ_1C0();
      return;
    }
  } while (bVar1 == 2);
  CC_6_OBJ_1C0();
  return;
}


