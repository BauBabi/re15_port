/* FUN_800386d0 @ 0x800386d0  (Ghidra headless, raw MIPS overlay) */

void FUN_800386d0(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 + 0x40);
  iVar1 = *(int *)(param_2 + 8) - (int)*(short *)(param_1 + 0x96);
  if (param_3 < 0) {
    iVar1 = iVar1 + param_4;
  }
  else {
    iVar1 = iVar1 - param_4;
  }
  *(int *)(param_1 + 0x40) = iVar1;
  *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + (*(int *)(param_1 + 0x40) - iVar3);
  uVar2 = FUN_8003873c(param_1);
  *(undefined4 *)(param_1 + 0x1e4) = uVar2;
  return;
}


