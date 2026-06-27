/* _sync @ 0x80092b84  (Ghidra headless, raw MIPS overlay) */

uint _sync(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    set_alarm();
    uVar1 = SYS_OBJ_2CDC();
    return uVar1;
  }
  uVar1 = DAT_800b2804 - DAT_800b2808 & 0x3f;
  if (uVar1 != 0) {
    _exeque();
  }
  if ((((*(uint *)PTR_DMA_GPU_CHCR_800b27e0 & 0x1000000) != 0) ||
      ((*(uint *)PTR_GPU_REG1_800b27d4 & 0x4000000) == 0)) && (uVar1 == 0)) {
    uVar1 = SYS_OBJ_2DD8();
    return uVar1;
  }
  return uVar1;
}


