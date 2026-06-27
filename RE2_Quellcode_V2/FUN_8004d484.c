/* FUN_8004d484 @ 0x8004d484  (Ghidra headless, raw MIPS overlay) */

bool FUN_8004d484(int *param_1,int param_2,short *param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = (*param_1 - ((int)*param_3 + (uint)(ushort)param_3[2] + param_2)) *
          -((uint)(ushort)param_3[3] + param_2 * 2);
  iVar1 = -((uint)(ushort)param_3[2] + param_2 * 2);
  if (iVar1 == 0) {
    trap(0x1c00);
  }
  if ((iVar1 == -1) && (iVar2 == -0x80000000)) {
    trap(0x1800);
  }
  return iVar2 / iVar1 < (int)(param_1[2] - ((int)param_3[1] + (uint)(ushort)param_3[3] + param_2));
}


