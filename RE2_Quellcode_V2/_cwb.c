/* _cwb @ 0x8009236c  (Ghidra headless, raw MIPS overlay) */

undefined4 _cwb(undefined4 *param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = param_2 + -1;
  *(undefined4 *)PTR_GPU_REG1_800b27d4 = 0x4000000;
  if (param_2 != 0) {
    do {
      uVar1 = *param_1;
      param_1 = param_1 + 1;
      iVar2 = iVar2 + -1;
      *(undefined4 *)PTR_GPU_REG0_800b27d0 = uVar1;
    } while (iVar2 != -1);
  }
  return 0;
}


