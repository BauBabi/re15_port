undefined4 SYS_OBJ_297C(void)

{
  int iVar1;
  undefined4 uVar2;
  
  _exeque();
  iVar1 = get_alarm();
  if (iVar1 == 0) {
    if (DAT_8007e454 != DAT_8007e458) {
      uVar2 = SYS_OBJ_297C();
      return uVar2;
    }
    do {
      if (((DMA_GPU_CHCR & 0x1000000) == 0) && ((GPU_REG1 & 0x4000000) != 0)) {
        uVar2 = SYS_OBJ_2A90();
        return uVar2;
      }
      iVar1 = get_alarm();
    } while (iVar1 == 0);
  }
  return 0xffffffff;
}
