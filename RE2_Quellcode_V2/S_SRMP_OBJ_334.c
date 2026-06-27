/* S_SRMP_OBJ_334 @ 0x80080a38  (Ghidra headless, raw MIPS overlay) */

undefined4 S_SRMP_OBJ_334(void)

{
  undefined *puVar1;
  undefined4 uVar2;
  int unaff_s2;
  uint unaff_s3;
  int unaff_s4;
  int unaff_s5;
  int unaff_s6;
  uint unaff_s7;
  int unaff_s8;
  undefined2 uStack00000022;
  int in_stack_00000058;
  
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  DAT_800ab2a8 = *(int *)(unaff_s2 + 0x10);
  uStack00000022 = (undefined2)((DAT_800ab2a8 * 0x8100) / 0x7f);
  if (unaff_s4 == 0) {
    if ((unaff_s5 != 0) || ((unaff_s3 & 2) != 0)) {
      *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x184) = *(undefined2 *)(unaff_s2 + 8);
      DAT_800ab2a0 = *(undefined2 *)(unaff_s2 + 8);
    }
    if ((unaff_s5 != 0) || ((unaff_s3 & 4) != 0)) {
      *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x186) = *(undefined2 *)(unaff_s2 + 10);
      DAT_800ab2a2 = *(undefined2 *)(unaff_s2 + 10);
      uVar2 = S_SRMP_OBJ_444();
      return uVar2;
    }
  }
  else {
    unaff_s7 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) >> 7 & 1;
    if ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) >> 7 & 1) != 0) {
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
           *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xff7f;
      uVar2 = S_SRMP_OBJ_420();
      return uVar2;
    }
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x184) = 0;
    *(undefined2 *)(puVar1 + 0x186) = 0;
    DAT_800ab2a0 = 0;
    DAT_800ab2a2 = 0;
  }
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


