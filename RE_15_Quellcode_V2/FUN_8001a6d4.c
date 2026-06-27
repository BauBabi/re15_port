uint FUN_8001a6d4(short param_1,short param_2,short param_3,short param_4)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  uVar1 = 0x400;
  iVar4 = (int)param_3 - (int)param_1;
  if (0 < (int)param_4 - (int)param_2) {
    uVar1 = 0xc00;
  }
  iVar3 = ((int)param_4 - (int)param_2) * 0x1000;
  if (iVar4 != 0) {
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (iVar3 == -0x80000000)) {
      trap(0x1800);
    }
    iVar2 = catan(iVar3 / iVar4);
    iVar3 = 0;
    if (iVar4 < 0) {
      iVar3 = 0x800;
    }
    uVar1 = iVar3 - (short)iVar2 & 0xfff;
  }
  return uVar1;
}
