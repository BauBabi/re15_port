undefined4 FUN_8002cabc(uint *param_1,uint *param_2,int param_3)

{
  bool bVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  short *psVar6;
  short *psVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  
  if ((*param_1 & 0x40) == 0) {
    if ((*param_2 & 2) != 0) {
      return 0;
    }
    psVar6 = (short *)param_2[0x1e];
    psVar7 = (short *)param_1[0x1e];
    iVar12 = ((int)*psVar6 + param_2[0xd]) - ((int)*(short *)param_1[0x1f] + param_1[0xd]);
    iVar11 = ((int)psVar6[2] + param_2[0xf]) - ((int)((short *)param_1[0x1f])[2] + param_1[0xf]);
    iVar9 = (uint)(ushort)psVar6[3] + (uint)(ushort)psVar7[3];
    iVar8 = (uint)(ushort)psVar6[5] + (uint)(ushort)psVar7[3];
    if ((uint)(iVar9 * 2) < (uint)(iVar12 + iVar9)) {
      return 0;
    }
    if ((uint)(iVar8 * 2) < (uint)(iVar11 + iVar8)) {
      return 0;
    }
    if (param_3 == 2) {
      return 1;
    }
    iVar10 = ((int)psVar6[1] + param_2[0xe]) - ((int)psVar7[1] + param_1[0xe]);
    iVar4 = (uint)(ushort)psVar7[4] + (uint)(ushort)psVar6[4];
    bVar1 = (uint)(iVar10 + iVar4) <= (uint)(iVar4 * 2);
    if (bVar1) {
      *(char *)((int)param_1 + 10) = (char)((int)(param_2 + 0x1ffd301a) * -0x6eb3e453 >> 2);
      if (param_1 == &DAT_800aca54) {
        uVar2 = 0x80;
      }
      else {
        uVar2 = (undefined1)((int)(param_1 + 0x1ffd4cf5) * 0x26e978d5 >> 2);
      }
      *(undefined1 *)((int)param_2 + 10) = uVar2;
      iVar5 = (int)psVar6[1];
      iVar3 = (int)psVar7[1];
      iVar13 = iVar12 * iVar8;
      if ((uint)(((*(short *)((int)param_2 + 0x42) + iVar5) -
                 (*(short *)((int)param_1 + 0x42) + iVar3)) + iVar4) <= (uint)(iVar4 * 2) != bVar1
          && bVar1) {
        if (param_3 == 0) {
          iVar3 = (iVar5 + param_2[0xe]) - iVar3;
          if (iVar10 < 0) {
            param_1[0xe] = iVar3 + 1 + iVar4;
            return 0;
          }
          param_1[0xe] = (iVar3 + -1) - iVar4;
          return 0;
        }
        iVar5 = (iVar3 + param_1[0xe]) - iVar5;
        if (iVar10 < 0) {
          param_2[0xe] = (iVar5 + -1) - iVar4;
          return 0;
        }
        param_2[0xe] = iVar5 + 1 + iVar4;
        return 0;
      }
      iVar4 = iVar11 * iVar9;
      if (iVar13 < 0) {
        iVar13 = -iVar13;
      }
      if (iVar4 < 0) {
        iVar4 = -iVar4;
      }
      if (iVar13 <= iVar4) {
        if (param_3 != 0) {
          if (iVar11 < 0) {
            iVar8 = -iVar8;
          }
          param_2[0xf] = (((int)psVar7[2] + param_1[0xf]) - (int)psVar6[2]) + iVar8;
          return 1;
        }
        if (-1 < iVar11) {
          iVar8 = -iVar8;
        }
        param_1[0xf] = (((int)psVar6[2] + param_2[0xf]) - (int)psVar7[2]) + iVar8;
        return 0;
      }
      if (param_3 != 0) {
        if (iVar12 < 0) {
          iVar9 = -iVar9;
        }
        param_2[0xd] = (((int)*psVar7 + param_1[0xd]) - (int)*psVar6) + iVar9;
        return 1;
      }
      iVar8 = ((int)*psVar6 + param_2[0xd]) - (int)*psVar7;
      if (iVar12 < 0) {
        param_1[0xd] = iVar8 + iVar9;
        return 0;
      }
      param_1[0xd] = iVar8 - iVar9;
      return 0;
    }
  }
  return 0;
}
