/* FUN_8004c858 @ 0x8004c858  (Ghidra headless, raw MIPS overlay) */

void FUN_8004c858(int param_1,int param_2,short *param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = (((uint)(ushort)param_3[2] + (int)*param_3 + (uint)*(ushort *)(param_1 + 0x90)) -
          *(int *)(param_1 + 0x84)) + 1;
  iVar1 = (((int)*param_3 - (uint)*(ushort *)(param_1 + 0x90)) - *(int *)(param_1 + 0x84)) + -1;
  if (-iVar1 < iVar3) {
    iVar3 = iVar1;
  }
  iVar1 = (((uint)(ushort)param_3[3] + (int)param_3[1] + param_2) - *(int *)(param_1 + 0x8c)) + 1;
  iVar2 = ((param_3[1] - param_2) - *(int *)(param_1 + 0x8c)) + -1;
  if (-iVar2 < iVar1) {
    iVar1 = iVar2;
  }
  FUN_8004ca68(param_1,param_1 + 0x118,iVar3,iVar1);
  return;
}


