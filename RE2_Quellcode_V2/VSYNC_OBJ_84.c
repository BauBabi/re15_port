/* VSYNC_OBJ_84 @ 0x80085f24  (Ghidra headless, raw MIPS overlay) */

void VSYNC_OBJ_84(void)

{
  uint uVar1;
  
  v_wait();
  uVar1 = *(uint *)PTR_GPU_REG1_800abbdc;
  v_wait(DAT_800acd00 + 1,1);
  if (((uVar1 & 0x400000) != 0) && (-1 < (int)(uVar1 ^ *(uint *)PTR_GPU_REG1_800abbdc))) {
    do {
    } while (((uVar1 ^ *(uint *)PTR_GPU_REG1_800abbdc) & 0x80000000) == 0);
  }
  DAT_800abbe8 = DAT_800acd00;
  DAT_800abbe4 = *(undefined4 *)PTR_TMR_HRETRACE_VAL_800abbe0;
  return;
}


