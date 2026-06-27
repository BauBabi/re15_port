/* _version @ 0x80092e6c  (Ghidra headless, raw MIPS overlay) */

undefined4 _version(uint param_1)

{
  undefined4 uVar1;
  
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x10000007;
  if ((*(uint *)PTR_GPU_REG0_800b27d0 & 0xffffff) == 2) {
    if ((param_1 & 8) != 0) {
      *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x9000001;
      uVar1 = SYS_OBJ_3060();
      return uVar1;
    }
    return 3;
  }
  *(uint *)PTR_GPU_REG0_800b27d0 = *(uint *)PTR_GPU_REG1_800b27d4 & 0x3fff | 0xe1001000;
  uVar1 = *(undefined4 *)PTR_GPU_REG0_800b27d0;
  if ((*(uint *)PTR_GPU_REG1_800b27d4 & 0x1000) == 0) {
    uVar1 = SYS_OBJ_3060();
    return uVar1;
  }
  if ((param_1 & 8) == 0) {
    uVar1 = SYS_OBJ_3060();
    return uVar1;
  }
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x20000504;
  uVar1 = SYS_OBJ_3060();
  return uVar1;
}


