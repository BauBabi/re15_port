/* FUN_8004dde8 @ 0x8004dde8  (Ghidra headless, raw MIPS overlay) */

bool FUN_8004dde8(int *param_1,int param_2,short *param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = (*param_1 - ((int)*param_3 + (uint)(ushort)param_3[2] + param_2)) *
          (param_2 * -2 - (uint)(ushort)param_3[3]);
  iVar1 = (uint)(ushort)param_3[2] + param_2 * 2;
  if (iVar1 == 0) {
    trap(0x1c00);
  }
  if ((iVar1 == -1) && (iVar2 == -0x80000000)) {
    trap(0x1800);
  }
  return param_1[2] - (param_3[1] - param_2) < iVar2 / iVar1;
}


