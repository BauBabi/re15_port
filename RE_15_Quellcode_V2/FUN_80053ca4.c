void FUN_80053ca4(int param_1)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  long lVar5;
  long lVar6;
  uint uVar7;
  uint uVar8;
  MATRIX local_50;
  VECTOR local_30;
  
  DAT_800b5288 = 0x1000;
  DAT_800b528c = 0;
  DAT_800b5290 = 0x1000;
  DAT_800b5294 = 0;
  DAT_800b5298 = 0x1000;
  DAT_800b529c = 0;
  DAT_800b52a0 = 0;
  DAT_800b52a4 = 0;
  iVar2 = *(int *)(param_1 + 0x10) - *(int *)(param_1 + 4);
  iVar3 = *(int *)(param_1 + 0x14) - *(int *)(param_1 + 8);
  iVar4 = *(int *)(param_1 + 0x18) - *(int *)(param_1 + 0xc);
  lVar5 = SquareRoot0(iVar2 * iVar2 + iVar3 * iVar3 + iVar4 * iVar4);
  iVar2 = (*(int *)(param_1 + 8) - *(int *)(param_1 + 0x14)) * 0x1000;
  if (lVar5 == 0) {
    trap(0x1c00);
  }
  if ((lVar5 == -1) && (iVar2 == -0x80000000)) {
    trap(0x1800);
  }
  iVar3 = *(int *)(param_1 + 0x10) - *(int *)(param_1 + 4);
  iVar4 = *(int *)(param_1 + 0x18) - *(int *)(param_1 + 0xc);
  sVar1 = (short)(iVar2 / lVar5);
  lVar6 = SquareRoot0(iVar3 * iVar3 + iVar4 * iVar4);
  uVar7 = (lVar6 << 0xc) / lVar5;
  if (lVar5 == 0) {
    trap(0x1c00);
  }
  if ((lVar5 == -1) && (lVar6 << 0xc == -0x80000000)) {
    trap(0x1800);
  }
  local_50.m[0][0] = 0x1000;
  local_50.m[0][1] = 0;
  local_50.m[0][2] = 0;
  local_50.m[1][0] = 0;
  local_50.t[0] = 0;
  local_50.t[1] = 0;
  local_50.t[2] = 0;
  local_50.m[2]._0_4_ = (uint)(ushort)-sVar1 << 0x10;
  local_50.m[1][2] = sVar1;
  local_50.m[1][1] = (short)uVar7;
  local_50._16_4_ = uVar7 & 0xffff;
  MulMatrix((MATRIX *)&DAT_800b5288,&local_50);
  if (lVar6 != 0) {
    iVar2 = (*(int *)(param_1 + 0x10) - *(int *)(param_1 + 4)) * 0x1000;
    uVar7 = iVar2 / lVar6;
    if (lVar6 == 0) {
      trap(0x1c00);
    }
    if ((lVar6 == -1) && (iVar2 == -0x80000000)) {
      trap(0x1800);
    }
    iVar2 = (*(int *)(param_1 + 0x18) - *(int *)(param_1 + 0xc)) * 0x1000;
    uVar8 = iVar2 / lVar6;
    if (lVar6 == 0) {
      trap(0x1c00);
    }
    if ((lVar6 == -1) && (iVar2 == -0x80000000)) {
      trap(0x1800);
    }
    local_50.m[1][1] = 0x1000;
    local_50.m[1][2] = 0;
    local_50.t[0] = 0;
    local_50.t[1] = 0;
    local_50.t[2] = 0;
    local_50.m[1][0] = 0;
    local_50.m[0][2] = -(short)uVar7;
    local_50.m[2]._0_4_ = uVar7 & 0xffff;
    local_50.m[0]._0_4_ = uVar8 & 0xffff;
    local_50._16_4_ = uVar8 & 0xffff;
    MulMatrix((MATRIX *)&DAT_800b5288,&local_50);
  }
  local_30.vx = -*(int *)(param_1 + 4);
  local_30.vy = -*(int *)(param_1 + 8);
  local_30.vz = -*(int *)(param_1 + 0xc);
  ApplyMatrixLV((MATRIX *)&DAT_800b5288,&local_30,(VECTOR *)&DAT_800b529c);
  return;
}
