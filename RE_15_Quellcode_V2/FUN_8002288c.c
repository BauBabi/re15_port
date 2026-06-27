void FUN_8002288c(int *param_1)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  
  if (*param_1 == 0) {
    piVar2 = param_1 + 2;
    piVar1 = param_1 + 8;
    iVar4 = param_1[1];
    *param_1 = 1;
    piVar3 = piVar2;
    do {
      iVar4 = iVar4 + -1;
      *piVar3 = (int)piVar2 + *piVar3;
      piVar3 = piVar3 + 7;
      piVar1[-4] = (int)piVar2 + piVar1[-4];
      *piVar1 = (int)piVar2 + *piVar1;
      piVar1[-2] = (int)piVar2 + piVar1[-2];
      piVar1 = piVar1 + 7;
    } while (iVar4 != 0);
  }
  return;
}
