/* S_SRMP_OBJ_444 @ 0x80080b48  (Ghidra headless, raw MIPS overlay) */

undefined4 S_SRMP_OBJ_444(void)

{
  int unaff_s4;
  int unaff_s6;
  int unaff_s7;
  int unaff_s8;
  int in_stack_00000058;
  
  if (((unaff_s4 != 0) || (unaff_s6 != 0)) || (unaff_s8 != 0)) {
    _spu_setReverbAttr(&stack0x00000010);
  }
  if (in_stack_00000058 != 0) {
    SpuClearReverbWorkArea(DAT_800ab29c);
  }
  if ((unaff_s4 != 0) && (_spu_FsetRXX(0xd1,DAT_800ab294,0), unaff_s7 != 0)) {
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 0x80;
  }
  return 0;
}


