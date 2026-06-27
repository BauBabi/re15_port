undefined4 SYS_OBJ_2994(void)

{
  undefined4 uVar1;
  int iVar2;
  
  if (DAT_8007e454 != DAT_8007e458) {
    uVar1 = SYS_OBJ_297C();
    return uVar1;
  }
  while (((DMA_GPU_CHCR & 0x1000000) != 0 || ((GPU_REG1 & 0x4000000) == 0))) {
    iVar2 = get_alarm();
    if (iVar2 != 0) {
      return 0xffffffff;
    }
  }
  uVar1 = SYS_OBJ_2A90();
  return uVar1;
}
