undefined4 S_SRMP_OBJ_45C(void)

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
    SpuClearReverbWorkArea(DAT_80076dd8);
  }
  if ((unaff_s4 != 0) && (_spu_FsetRXX(0xd1,DAT_80076dd0,0), unaff_s7 != 0)) {
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 0x80;
  }
  return 0;
}
