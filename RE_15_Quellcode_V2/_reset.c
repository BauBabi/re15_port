undefined4 _reset(uint param_1)

{
  undefined4 uVar1;
  
  DAT_8007e464 = SetIntrMask(0);
  DAT_8007e458 = 0;
  DAT_8007e454 = 0;
  if ((param_1 & 7) == 0) {
    DMA_GPU_CHCR = 0x401;
    DMA_DPCR = DMA_DPCR | 0x800;
    GPU_REG1 = 0;
    memset("",'\0',0x100);
    memset("",'\0',0x1800);
    uVar1 = SYS_OBJ_2924();
    return uVar1;
  }
  if ((param_1 & 7) != 1) {
    uVar1 = SYS_OBJ_2924();
    return uVar1;
  }
  DMA_GPU_CHCR = 0x401;
  DMA_DPCR = DMA_DPCR | 0x800;
  GPU_REG1 = 0x1000000;
  SetIntrMask(DAT_8007e464);
  uVar1 = 0;
  if ((param_1 & 7) == 0) {
    uVar1 = _version(param_1);
  }
  return uVar1;
}
