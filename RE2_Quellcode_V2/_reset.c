/* _reset @ 0x80092a28  (Ghidra headless, raw MIPS overlay) */

undefined4 _reset(uint param_1)

{
  undefined4 uVar1;
  uint uVar2;
  
  DAT_800b2814 = SetIntrMask(0);
  DAT_800b2808 = 0;
  DAT_800b2804 = 0;
  uVar2 = param_1 & 7;
  if (uVar2 != 1) {
    if (uVar2 < 2) {
      if (uVar2 != 0) {
        uVar1 = SYS_OBJ_2C6C();
        return uVar1;
      }
SYS_OBJ_2BBC:
      *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x401;
      *(uint *)PTR_DMA_DPCR_800b27f0 = *(uint *)PTR_DMA_DPCR_800b27f0 | 0x800;
      *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0;
      memset("",'\0',0x100);
      memset("",'\0',0x1800);
      uVar1 = SYS_OBJ_2C6C();
      return uVar1;
    }
    if (uVar2 != 3) {
      if (uVar2 == 5) goto SYS_OBJ_2BBC;
      goto code_r0x80092b4c;
    }
  }
  *(undefined4 *)PTR_DMA_GPU_CHCR_800b27e0 = 0x401;
  *(uint *)PTR_DMA_DPCR_800b27f0 = *(uint *)PTR_DMA_DPCR_800b27f0 | 0x800;
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x2000000;
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x1000000;
code_r0x80092b4c:
  SetIntrMask(DAT_800b2814);
  uVar1 = 0;
  if ((param_1 & 7) == 0) {
    uVar1 = _version(param_1);
  }
  return uVar1;
}


