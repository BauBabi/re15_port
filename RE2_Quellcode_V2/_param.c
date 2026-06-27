/* _param @ 0x80092408  (Ghidra headless, raw MIPS overlay) */

uint _param(uint param_1)

{
  *(uint *)PTR_GPU_REG1_800b27d4 = param_1 | 0x10000000;
  return *(uint *)PTR_GPU_REG0_800b27d0 & 0xffffff;
}


