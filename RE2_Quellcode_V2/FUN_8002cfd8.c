/* FUN_8002cfd8 @ 0x8002cfd8  (Ghidra headless, raw MIPS overlay) */

void FUN_8002cfd8(int param_1)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  
  iVar4 = *(int *)(param_1 + 4);
  piVar2 = (int *)(param_1 + 8);
  piVar1 = (int *)(param_1 + 0x20);
  piVar3 = piVar2;
  do {
    iVar4 = iVar4 + -1;
    *piVar3 = *piVar3 + (int)piVar2;
    piVar3 = piVar3 + 7;
    piVar1[-4] = piVar1[-4] + (int)piVar2;
    *piVar1 = *piVar1 + (int)piVar2;
    piVar1[-2] = piVar1[-2] + (int)piVar2;
    piVar1 = piVar1 + 7;
  } while (iVar4 != 0);
  return;
}


