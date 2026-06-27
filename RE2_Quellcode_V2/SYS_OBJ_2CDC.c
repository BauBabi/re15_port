/* SYS_OBJ_2CDC @ 0x80092bbc  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_2CDC(void)

{
  undefined4 uVar1;
  int iVar2;
  
  if (DAT_800b2804 != DAT_800b2808) {
    uVar1 = SYS_OBJ_2CC4();
    return uVar1;
  }
  while (((*(uint *)PTR_DMA_GPU_CHCR_800b27e0 & 0x1000000) != 0 ||
         ((*(uint *)PTR_GPU_REG1_800b27d4 & 0x4000000) == 0))) {
    iVar2 = get_alarm();
    if (iVar2 != 0) {
      return 0xffffffff;
    }
  }
  uVar1 = SYS_OBJ_2DD8();
  return uVar1;
}


