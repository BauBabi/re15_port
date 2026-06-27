undefined4 S_SRMP_OBJ_38C(void)

{
  undefined4 uVar1;
  int unaff_s2;
  uint unaff_s3;
  int unaff_s4;
  int unaff_s5;
  int unaff_s6;
  uint unaff_s7;
  int unaff_s8;
  int in_stack_00000058;
  
  if (unaff_s4 == 0) {
    if ((unaff_s5 != 0) || ((unaff_s3 & 2) != 0)) {
      SPU_REVERB_OUT_L = *(word *)(unaff_s2 + 8);
      DAT_80076ddc = *(undefined2 *)(unaff_s2 + 8);
    }
    if ((unaff_s5 != 0) || ((unaff_s3 & 4) != 0)) {
      SPU_REVERB_OUT_R = *(word *)(unaff_s2 + 10);
      DAT_80076dde = *(undefined2 *)(unaff_s2 + 10);
      uVar1 = S_SRMP_OBJ_45C();
      return uVar1;
    }
  }
  else {
    unaff_s7 = SPU_CTRL_REG_CPUCNT >> 7 & 1;
    if ((SPU_CTRL_REG_CPUCNT >> 7 & 1) != 0) {
      SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xff7f;
      uVar1 = S_SRMP_OBJ_438();
      return uVar1;
    }
    SPU_REVERB_OUT_L = 0;
    SPU_REVERB_OUT_R = 0;
    DAT_80076ddc = 0;
    DAT_80076dde = 0;
  }
  if (((unaff_s4 != 0) || (unaff_s6 != 0)) || (unaff_s8 != 0)) {
    _spu_setReverbAttr(&stack0x00000010);
  }
  if (in_stack_00000058 != 0) {
    SpuClearReverbWorkArea(DAT_80076dd8);
  }
  if ((unaff_s4 != 0) && (_spu_FsetRXX(0xd1,DAT_80076dd0,0), unaff_s7 != 0)) {
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 0x80;
  }
  return 0;
}
