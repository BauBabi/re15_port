undefined4 FUN_8003d6a8(ushort *param_1,int *param_2,undefined4 param_3,uint param_4)

{
  long lVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  uVar4 = (uint)(*param_1 >> 1);
  iVar7 = ((int)**(short **)(DAT_800ac784 + 0x7c) + *param_2) - ((int)(short)param_1[2] + uVar4);
  iVar6 = ((int)(*(short **)(DAT_800ac784 + 0x7c))[2] + param_2[2]) -
          ((int)(short)param_1[3] + uVar4);
  lVar1 = SquareRoot0(iVar7 * iVar7 + iVar6 * iVar6);
  iVar5 = (uVar4 + (param_4 & 0xffff)) - lVar1;
  if (iVar5 < 1) {
    uVar2 = 0;
  }
  else {
    iVar3 = lVar1 + 1;
    if (iVar3 == 0) {
      trap(0x1c00);
    }
    if ((iVar3 == -1) && (iVar7 * iVar5 == -0x80000000)) {
      trap(0x1800);
    }
    if (iVar3 == 0) {
      trap(0x1c00);
    }
    if ((iVar3 == -1) && (iVar6 * iVar5 == -0x80000000)) {
      trap(0x1800);
    }
    uVar2 = 1;
    *param_2 = (iVar7 * iVar5) / iVar3 + *param_2;
    param_2[2] = (iVar6 * iVar5) / iVar3 + param_2[2];
  }
  return uVar2;
}
