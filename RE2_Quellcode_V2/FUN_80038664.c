/* FUN_80038664 @ 0x80038664  (Ghidra headless, raw MIPS overlay) */

void FUN_80038664(int param_1,int *param_2,int param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 + 0x38);
  iVar1 = *param_2 - (int)*(short *)(param_1 + 0x94);
  if (param_3 < 0) {
    iVar1 = iVar1 + param_4;
  }
  else {
    iVar1 = iVar1 - param_4;
  }
  *(int *)(param_1 + 0x38) = iVar1;
  *(int *)(param_1 + 0x84) = *(int *)(param_1 + 0x84) + (*(int *)(param_1 + 0x38) - iVar3);
  uVar2 = FUN_8003873c(param_1);
  *(undefined4 *)(param_1 + 0x1e4) = uVar2;
  return;
}


