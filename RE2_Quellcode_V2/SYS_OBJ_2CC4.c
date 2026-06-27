/* SYS_OBJ_2CC4 @ 0x80092ba4  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_2CC4(void)

{
  int iVar1;
  undefined4 uVar2;
  
  _exeque();
  iVar1 = get_alarm();
  if (iVar1 == 0) {
    if (DAT_800b2804 != DAT_800b2808) {
      uVar2 = SYS_OBJ_2CC4();
      return uVar2;
    }
    do {
      if (((*(uint *)PTR_DMA_GPU_CHCR_800b27e0 & 0x1000000) == 0) &&
         ((*(uint *)PTR_GPU_REG1_800b27d4 & 0x4000000) != 0)) {
        uVar2 = SYS_OBJ_2DD8();
        return uVar2;
      }
      iVar1 = get_alarm();
    } while (iVar1 == 0);
  }
  return 0xffffffff;
}


