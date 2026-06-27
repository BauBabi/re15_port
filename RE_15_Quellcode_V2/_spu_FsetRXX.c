void _spu_FsetRXX(int param_1,uint param_2,int param_3)

{
  if (param_3 == 0) {
    *(short *)((int)&VOICE_00_LEFT_RIGHT + param_1 * 2) = (short)param_2;
    SPU_OBJ_1034();
    return;
  }
  *(short *)((int)&VOICE_00_LEFT_RIGHT + param_1 * 2) = (short)(param_2 >> (DAT_80076d80 & 0x1f));
  return;
}
