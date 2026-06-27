/* FUN_8004ea14 @ 0x8004ea14  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8004ea14(int *param_1,int param_2,ushort *param_3)

{
  ushort uVar1;
  ushort uVar2;
  bool bVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  
  uVar1 = *param_3;
  uVar2 = param_3[1];
  iVar10 = param_1[2];
  iVar5 = *param_1;
  iVar13 = (uint)param_3[2] + (uint)uVar1;
  iVar12 = (uint)param_3[3] + (uint)uVar2;
  iVar8 = (int)(((uint)uVar2 + (uint)(param_3[3] >> 1)) * 0x10000) >> 0x10;
  uVar11 = iVar10 - iVar8;
  iVar9 = (int)(((uint)uVar1 + (uint)(param_3[2] >> 1)) * 0x10000) >> 0x10;
  uVar7 = iVar5 - iVar9;
  uVar6 = uVar11 >> 0x1e & 2 | uVar7 >> 0x1f;
  if (uVar6 == 1) {
    iVar5 = (((iVar12 * 0x10000 >> 0x10) + param_2) - iVar8) * (iVar5 - ((short)uVar1 - param_2));
    iVar9 = iVar9 - ((short)uVar1 - param_2);
    if (iVar9 == 0) {
      trap(0x1c00);
    }
    if ((iVar9 == -1) && (iVar5 == -0x80000000)) {
      trap(0x1800);
    }
    bVar3 = (int)uVar11 < iVar5 / iVar9;
LAB_8004ec3c:
    uVar4 = 1;
    if (!bVar3) {
      uVar4 = 0;
    }
  }
  else {
    if (uVar6 < 2) {
      if (uVar6 != 0) {
        return 0;
      }
      iVar5 = (iVar12 * 0x10000 >> 0x10) + param_2;
      iVar8 = (iVar8 - iVar5) * uVar7;
      iVar9 = ((iVar13 * 0x10000 >> 0x10) + param_2) - iVar9;
      if (iVar9 == 0) {
        trap(0x1c00);
      }
      if ((iVar9 == -1) && (iVar8 == -0x80000000)) {
        trap(0x1800);
      }
      bVar3 = iVar10 - iVar5 < iVar8 / iVar9;
    }
    else {
      if (uVar6 != 2) {
        if (uVar6 != 3) {
          return 0;
        }
        iVar5 = (((short)uVar2 - param_2) - iVar8) * (iVar5 - ((short)uVar1 - param_2));
        iVar9 = iVar9 - ((short)uVar1 - param_2);
        if (iVar9 == 0) {
          trap(0x1c00);
        }
        if ((iVar9 == -1) && (iVar5 == -0x80000000)) {
          trap(0x1800);
        }
        bVar3 = iVar5 / iVar9 < (int)uVar11;
        goto LAB_8004ec3c;
      }
      iVar5 = (iVar8 - ((short)uVar2 - param_2)) * uVar7;
      iVar9 = ((iVar13 * 0x10000 >> 0x10) + param_2) - iVar9;
      if (iVar9 == 0) {
        trap(0x1c00);
      }
      if ((iVar9 == -1) && (iVar5 == -0x80000000)) {
        trap(0x1800);
      }
      bVar3 = iVar5 / iVar9 < iVar10 - ((short)uVar2 - param_2);
    }
    uVar4 = 0;
    if (bVar3) {
      uVar4 = 1;
    }
  }
  return uVar4;
}


