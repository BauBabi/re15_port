undefined4 FUN_8003da78(ushort *param_1,int param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = *(int *)(param_2 + 0x34);
  param_3 = param_3 & 0xffff;
  iVar1 = **(short **)(param_2 + 0x78) + iVar7;
  uVar5 = (((uint)*param_1 + (int)(short)param_1[2] + param_3) - iVar1) + 0x12;
  uVar3 = (((int)(short)param_1[2] - param_3) - iVar1) - 0x12;
  if ((int)-uVar3 < (int)uVar5) {
    uVar5 = uVar3;
  }
  iVar6 = *(int *)(param_2 + 0x3c);
  iVar1 = (*(short **)(param_2 + 0x78))[2] + iVar6;
  uVar3 = (((uint)param_1[1] + (int)(short)param_1[3] + param_3) - iVar1) + 0x12;
  uVar4 = (((int)(short)param_1[3] - param_3) - iVar1) - 0x12;
  if ((int)-uVar4 < (int)uVar3) {
    uVar3 = uVar4;
  }
  uVar4 = (uVar5 ^ iVar7 - *(short *)(param_2 + 0x40)) >> 0x1f |
          (uVar3 ^ iVar6 - *(short *)(param_2 + 0x44)) >> 0x1e & 2;
  DAT_800b8294 = param_2;
  if (uVar4 == 2) {
    uVar4 = uVar3;
    if ((int)uVar3 < 0) {
      uVar4 = -uVar3;
    }
    if ((uVar4 + 400 & 0xffff) < 0x321) {
      *(uint *)(param_2 + 0x3c) = uVar3 + iVar6;
      return 1;
    }
LAB_8003dbc8:
    uVar4 = uVar5;
    if ((int)uVar5 < 0) {
      uVar4 = -uVar5;
    }
    uVar2 = uVar3;
    if ((int)uVar3 < 0) {
      uVar2 = -uVar3;
    }
    if ((int)uVar4 < (int)uVar2) {
      *(uint *)(param_2 + 0x34) = uVar5 + *(int *)(param_2 + 0x34);
    }
    else {
      *(uint *)(param_2 + 0x3c) = uVar3 + *(int *)(param_2 + 0x3c);
    }
  }
  else {
    if (uVar4 < 3) {
      if (uVar4 == 1) {
        uVar4 = uVar5;
        if ((int)uVar5 < 0) {
          uVar4 = -uVar5;
        }
        if ((uVar4 + 400 & 0xffff) < 0x321) {
          *(uint *)(param_2 + 0x34) = uVar5 + iVar7;
          return 1;
        }
        goto LAB_8003dbc8;
      }
    }
    else if (uVar4 == 3) goto LAB_8003dbc8;
    if ((*(int *)(param_2 + 0x34) != (int)*(short *)(param_2 + 0x40)) ||
       (*(int *)(param_2 + 0x3c) != (int)*(short *)(param_2 + 0x44))) {
      *(int *)(param_2 + 0x34) = (int)*(short *)(param_2 + 0x40);
      *(int *)(param_2 + 0x3c) = (int)*(short *)(param_2 + 0x44);
      return 0;
    }
    uVar4 = uVar5;
    if ((int)uVar5 < 0) {
      uVar4 = -uVar5;
    }
    uVar2 = uVar3;
    if ((int)uVar3 < 0) {
      uVar2 = -uVar3;
    }
    if ((int)uVar4 < (int)uVar2) {
      *(uint *)(param_2 + 0x34) = uVar5 + *(int *)(param_2 + 0x34);
    }
    else {
      *(uint *)(param_2 + 0x3c) = uVar3 + *(int *)(param_2 + 0x3c);
    }
  }
  return 1;
}
