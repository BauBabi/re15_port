undefined4 FUN_8003bca8(ushort *param_1,int *param_2,short *param_3,uint param_4)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  
  param_4 = param_4 & 0xffff;
  iVar8 = *param_2;
  iVar2 = **(short **)(DAT_800ac784 + 0x7c) + iVar8;
  uVar7 = (((uint)*param_1 + (int)(short)param_1[2] + param_4) - iVar2) + 0x12;
  uVar5 = (((int)(short)param_1[2] - param_4) - iVar2) - 0x12;
  if ((int)-uVar5 < (int)uVar7) {
    uVar7 = uVar5;
  }
  iVar2 = param_2[2];
  iVar3 = (*(short **)(DAT_800ac784 + 0x7c))[2] + iVar2;
  uVar5 = (((uint)param_1[1] + (int)(short)param_1[3] + param_4) - iVar3) + 0x12;
  uVar6 = (((int)(short)param_1[3] - param_4) - iVar3) - 0x12;
  if ((int)-uVar6 < (int)uVar5) {
    uVar5 = uVar6;
  }
  uVar6 = (uVar7 ^ iVar8 - *param_3) >> 0x1f | (uVar5 ^ iVar2 - param_3[2]) >> 0x1e & 2;
  if (uVar6 == 2) {
    uVar6 = uVar5;
    if ((int)uVar5 < 0) {
      uVar6 = -uVar5;
    }
    if ((uVar6 + 400 & 0xffff) < 0x321) {
LAB_8003be84:
      param_2[2] = uVar5 + iVar2;
    }
    else {
LAB_8003bdf0:
      uVar6 = uVar7;
      if ((int)uVar7 < 0) {
        uVar6 = -uVar7;
      }
      uVar4 = uVar5;
      if ((int)uVar5 < 0) {
        uVar4 = -uVar5;
      }
      if ((int)uVar6 < (int)uVar4) {
        iVar8 = uVar7 + *param_2;
LAB_8003bdc4:
        *param_2 = iVar8;
      }
      else {
        param_2[2] = uVar5 + param_2[2];
      }
    }
    uVar1 = 1;
  }
  else {
    if (uVar6 < 3) {
      if (uVar6 == 1) {
        uVar6 = uVar7;
        if ((int)uVar7 < 0) {
          uVar6 = -uVar7;
        }
        iVar8 = uVar7 + iVar8;
        if (800 < (uVar6 + 400 & 0xffff)) goto LAB_8003bdf0;
        goto LAB_8003bdc4;
      }
    }
    else if (uVar6 == 3) goto LAB_8003bdf0;
    iVar8 = (int)*param_3;
    if (*param_2 == iVar8) {
      iVar2 = param_2[2];
      if (iVar2 == param_3[2]) {
        uVar6 = uVar7;
        if ((int)uVar7 < 0) {
          uVar6 = -uVar7;
        }
        uVar4 = uVar5;
        if ((int)uVar5 < 0) {
          uVar4 = -uVar5;
        }
        iVar8 = uVar7 + *param_2;
        if ((int)uVar6 < (int)uVar4) goto LAB_8003bdc4;
        goto LAB_8003be84;
      }
      iVar8 = (int)*param_3;
    }
    *param_2 = iVar8;
    uVar1 = 0;
    param_2[2] = (int)param_3[2];
  }
  return uVar1;
}
