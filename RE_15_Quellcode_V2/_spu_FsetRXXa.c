uint _spu_FsetRXXa(int param_1,uint param_2)

{
  uint uVar1;
  
  if (DAT_80076d7c != 0) {
    if (DAT_80076d84 == 0) {
      trap(0x1c00);
    }
    if (param_2 % DAT_80076d84 != 0) {
      param_2 = param_2 + DAT_80076d84 & ~DAT_80076d88;
    }
  }
  if (param_1 != -2) {
    if (param_1 == -1) {
      uVar1 = SPU_OBJ_10D8();
      return uVar1;
    }
    *(short *)((int)&VOICE_00_LEFT_RIGHT + param_1 * 2) = (short)(param_2 >> (DAT_80076d80 & 0x1f));
    return param_2;
  }
  uVar1 = SPU_OBJ_10D8();
  return uVar1;
}
