/* _ctl @ 0x80092330  (Ghidra headless, raw MIPS overlay) */

void _ctl(uint param_1)

{
  *(uint *)PTR_GPU_REG1_800b27d4 = param_1;
  (&DAT_800c3f5c)[param_1 >> 0x18] = (char)param_1;
  return;
}


