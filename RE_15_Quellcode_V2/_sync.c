uint _sync(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    set_alarm();
    uVar1 = SYS_OBJ_2994();
    return uVar1;
  }
  uVar1 = DAT_8007e454 - DAT_8007e458 & 0x3f;
  if (uVar1 != 0) {
    _exeque();
  }
  if ((((DMA_GPU_CHCR & 0x1000000) != 0) || ((GPU_REG1 & 0x4000000) == 0)) && (uVar1 == 0)) {
    uVar1 = SYS_OBJ_2A90();
    return uVar1;
  }
  return uVar1;
}
