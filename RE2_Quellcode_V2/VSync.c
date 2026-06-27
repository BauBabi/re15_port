/* VSync @ 0x80085ea0  (Ghidra headless, raw MIPS overlay) */

int VSync(int mode)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  uVar3 = *(undefined4 *)PTR_GPU_REG1_800abbdc;
  uVar1 = *(int *)PTR_TMR_HRETRACE_VAL_800abbe0 - DAT_800abbe4;
  if (mode < 0) {
    iVar2 = VSYNC_OBJ_130();
    return iVar2;
  }
  if (mode != 1) {
    if (0 < mode) {
      iVar2 = VSYNC_OBJ_84();
      return iVar2;
    }
    iVar2 = 0;
    if (0 < mode) {
      iVar2 = mode + -1;
    }
    v_wait(DAT_800abbe8,iVar2);
    uVar4 = *(uint *)PTR_GPU_REG1_800abbdc;
    v_wait(DAT_800acd00 + 1,1);
    if (((uVar4 & 0x400000) != 0) && (-1 < (int)(uVar4 ^ *(uint *)PTR_GPU_REG1_800abbdc))) {
      do {
      } while (((uVar4 ^ *(uint *)PTR_GPU_REG1_800abbdc) & 0x80000000) == 0);
    }
    DAT_800abbe8 = DAT_800acd00;
    DAT_800abbe4 = *(int *)PTR_TMR_HRETRACE_VAL_800abbe0;
  }
  return uVar1 & 0xffff;
}


