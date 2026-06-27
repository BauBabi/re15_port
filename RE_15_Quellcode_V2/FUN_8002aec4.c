undefined1 FUN_8002aec4(uint *param_1,uint *param_2)

{
  ushort uVar1;
  ushort uVar2;
  short sVar3;
  byte bVar4;
  short sVar5;
  long lVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  ushort uVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  short *psVar18;
  short *psVar19;
  int iVar20;
  ushort local_38;
  ushort local_30;
  
  uVar8 = *param_2;
  if (((*param_1 | uVar8) & 2) != 0) {
    return 0;
  }
  if ((*param_1 & uVar8 & 0x1000) != 0) {
    return 0;
  }
  if ((uVar8 & 4) != 0) {
    return 0;
  }
  uVar17 = param_1[0x1e];
  uVar8 = param_2[0x1e];
  local_38 = *(ushort *)(uVar17 + 6);
  local_30 = *(ushort *)(uVar8 + 6);
  psVar19 = (short *)param_1[0x1f];
  psVar18 = (short *)param_2[0x1f];
  if (*(short *)(uVar17 + 6) != *(short *)(uVar17 + 10)) {
    lVar6 = ratan2(((int)psVar18[2] + param_2[0xf]) - ((int)psVar19[2] + param_1[0xf]),
                   ((int)*psVar18 + param_2[0xd]) - ((int)*psVar19 + param_1[0xd]));
    uVar9 = lVar6 - (uint)*(ushort *)((int)param_1 + 0x6a);
    uVar13 = (ushort)uVar9 & 0xfff;
    if ((uVar9 & 0xc00) == 0xc00) {
      sVar5 = 0x1000;
LAB_8002aff0:
      uVar13 = sVar5 - uVar13;
    }
    else if ((uVar9 & 0x800) == 0) {
      sVar5 = 0x800;
      if ((uVar9 & 0x400) != 0) goto LAB_8002aff0;
    }
    else {
      uVar13 = uVar13 - 0x800;
    }
    uVar1 = *(ushort *)(uVar17 + 10);
    uVar2 = *(ushort *)(uVar17 + 6);
    iVar7 = rsin((int)(short)uVar13);
    local_38 = uVar2 + (short)((int)(((uint)uVar1 - (uint)uVar2) * iVar7) >> 0xc);
    if ((uint)uVar1 < (uint)uVar2) {
      uVar1 = *(ushort *)(uVar17 + 6);
      local_38 = *(ushort *)(uVar17 + 10);
      iVar7 = rcos((int)(short)uVar13);
      local_38 = local_38 + (short)((int)(((uint)uVar1 - (uint)local_38) * iVar7) >> 0xc);
    }
  }
  if (*(short *)(uVar8 + 6) == *(short *)(uVar8 + 10)) goto LAB_8002b164;
  lVar6 = ratan2(((int)psVar19[2] + param_1[0xf]) - ((int)psVar18[2] + param_2[0xf]),
                 ((int)*psVar19 + param_1[0xd]) - ((int)*psVar18 + param_2[0xd]));
  uVar9 = lVar6 - (uint)*(ushort *)((int)param_2 + 0x6a);
  uVar13 = (ushort)uVar9 & 0xfff;
  if ((uVar9 & 0xc00) == 0xc00) {
    sVar5 = 0x1000;
LAB_8002b0f4:
    uVar13 = sVar5 - uVar13;
  }
  else if ((uVar9 & 0x800) == 0) {
    sVar5 = 0x800;
    if ((uVar9 & 0x400) != 0) goto LAB_8002b0f4;
  }
  else {
    uVar13 = uVar13 - 0x800;
  }
  uVar1 = *(ushort *)(uVar8 + 10);
  uVar2 = *(ushort *)(uVar8 + 6);
  iVar7 = rsin((int)(short)uVar13);
  local_30 = uVar2 + (short)((int)(((uint)uVar1 - (uint)uVar2) * iVar7) >> 0xc);
  if ((uint)uVar1 < (uint)uVar2) {
    uVar1 = *(ushort *)(uVar8 + 6);
    local_30 = *(ushort *)(uVar8 + 10);
    iVar7 = rcos((int)(short)uVar13);
    local_30 = local_30 + (short)((int)(((uint)uVar1 - (uint)local_30) * iVar7) >> 0xc);
  }
LAB_8002b164:
  iVar7 = (uint)local_38 + (uint)local_30;
  iVar15 = ((int)*psVar18 + param_2[0xd]) - ((int)*psVar19 + param_1[0xd]);
  if ((uint)(iVar7 * 2) < (uint)(iVar15 + iVar7)) {
    return 0;
  }
  iVar16 = ((int)psVar18[2] + param_2[0xf]) - ((int)psVar19[2] + param_1[0xf]);
  if ((uint)(iVar7 * 2) < (uint)(iVar16 + iVar7)) {
    return 0;
  }
  lVar6 = SquareRoot0(iVar15 * iVar15 + iVar16 * iVar16);
  iVar7 = iVar7 - lVar6;
  if (iVar7 < 1) {
    return 0;
  }
  iVar11 = ((int)psVar18[1] + param_2[0xe]) - ((int)psVar19[1] + param_1[0xe]);
  iVar10 = (uint)*(ushort *)(uVar8 + 8) + (uint)*(ushort *)(uVar17 + 8);
  if (-iVar10 < iVar11) {
    if (iVar11 < iVar10) {
      iVar14 = lVar6 + 1;
      iVar11 = (iVar15 * iVar7) / iVar14;
      if (iVar14 == 0) {
        trap(0x1c00);
      }
      if ((iVar14 == -1) && (iVar15 * iVar7 == -0x80000000)) {
        trap(0x1800);
      }
      iVar12 = ((int)*(short *)((int)param_2 + 0x42) + (int)psVar18[1]) -
               ((int)psVar19[1] + param_1[0xe]);
      iVar20 = (iVar16 * iVar7) / iVar14;
      if (iVar14 == 0) {
        trap(0x1c00);
      }
      if ((iVar14 == -1) && (iVar16 * iVar7 == -0x80000000)) {
        trap(0x1800);
      }
      if ((iVar12 <= -iVar10) || (iVar10 <= iVar12)) {
        sVar5 = (short)param_1[0xd];
        sVar3 = (short)param_1[0xf];
        if ((((short)param_2[0x10] < sVar5) && (sVar5 < (short)param_2[0xd])) ||
           ((sVar5 < (short)param_2[0x10] && ((short)param_2[0xd] < sVar5)))) {
          iVar10 = -(iVar15 * iVar7) / iVar14;
          if (iVar14 == 0) {
            trap(0x1c00);
          }
          if ((iVar14 == -1) && (-(iVar15 * iVar7) == -0x80000000)) {
            trap(0x1800);
          }
          if (iVar10 < 1) {
            iVar11 = (uint)local_38 * -2;
          }
          else {
            iVar11 = (uint)local_38 << 1;
          }
          iVar11 = iVar11 - iVar10;
        }
        if ((((short)param_2[0x11] < sVar3) && (sVar3 < (short)param_2[0xf])) ||
           ((sVar3 < (short)param_2[0x11] && ((short)param_2[0xf] < sVar3)))) {
          iVar15 = -(iVar16 * iVar7) / iVar14;
          if (iVar14 == 0) {
            trap(0x1c00);
          }
          if ((iVar14 == -1) && (-(iVar16 * iVar7) == -0x80000000)) {
            trap(0x1800);
          }
          if (iVar15 < 1) {
            iVar20 = (uint)local_38 * -2;
          }
          else {
            iVar20 = (uint)local_38 << 1;
          }
          iVar20 = iVar20 - iVar15;
        }
      }
      param_2[0xd] = iVar11 + param_2[0xd];
      param_2[0xf] = iVar20 + param_2[0xf];
      if (param_1 == &DAT_800aca54) {
        bVar4 = *(byte *)((int)param_2 + 0x1c2) | 1;
      }
      else {
        param_2[0x6b] = (uint)param_1;
        bVar4 = *(byte *)((int)param_2 + 0x1c2) | 2;
      }
      *(byte *)((int)param_2 + 0x1c2) = bVar4;
      return 1;
    }
    return 0;
  }
  return 0;
}
