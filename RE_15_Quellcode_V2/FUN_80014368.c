undefined4 FUN_80014368(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)*(short *)(param_2 + 4);
  iVar3 = (int)*(short *)(param_2 + 6);
  iVar4 = param_1[2] - iVar3;
  iVar5 = *(short *)(param_2 + 8) - iVar8;
  iVar1 = *param_1 - iVar8;
  iVar2 = *(short *)(param_2 + 10) - iVar3;
  iVar6 = *(short *)(param_2 + 0x10) - iVar8;
  iVar7 = *(short *)(param_2 + 0x12) - iVar3;
  if (iVar5 * iVar4 <= iVar2 * iVar1) {
    if (iVar6 * iVar4 < iVar7 * iVar1) {
      return 0;
    }
    iVar3 = *(short *)(param_2 + 0xe) - iVar3;
    iVar8 = *(short *)(param_2 + 0xc) - iVar8;
    if (((iVar2 - iVar3) * (iVar1 - iVar8) <= (iVar5 - iVar8) * (iVar4 - iVar3)) &&
       ((iVar6 - iVar8) * (iVar4 - iVar3) <= (iVar7 - iVar3) * (iVar1 - iVar8))) {
      return 1;
    }
  }
  return 0;
}
