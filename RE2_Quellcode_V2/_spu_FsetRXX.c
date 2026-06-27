/* _spu_FsetRXX @ 0x80078fac  (Ghidra headless, raw MIPS overlay) */

void _spu_FsetRXX(int param_1,uint param_2,int param_3)

{
  if (param_3 == 0) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_1 * 2) = (short)param_2;
    SPU_OBJ_98C();
    return;
  }
  *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_1 * 2) =
       (short)(param_2 >> (DAT_800ab244 & 0x1f));
  return;
}


