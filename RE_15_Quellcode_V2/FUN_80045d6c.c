uint FUN_80045d6c(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  param_3 = param_3 - param_1;
  if (param_3 == 0) {
    uVar2 = 0x400;
    if (0 < param_4 - param_2) {
      uVar2 = 0xc00;
    }
  }
  else {
    iVar3 = (param_4 - param_2) * 0x1000;
    if (param_3 == 0) {
      trap(0x1c00);
    }
    if ((param_3 == -1) && (iVar3 == -0x80000000)) {
      trap(0x1800);
    }
    iVar1 = catan(iVar3 / param_3);
    iVar3 = 0;
    if (param_3 < 0) {
      iVar3 = 0x800;
    }
    uVar2 = iVar3 - iVar1 & 0xfff;
  }
  return uVar2;
}
