undefined4 FUN_8002cdf4(uint *param_1,uint *param_2)

{
  bool bVar1;
  undefined4 uVar2;
  uint uVar3;
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
  int iVar14;
  int iVar15;
  int iVar16;
  
  uVar2 = 0;
  if (((*param_1 | *param_2) & 0x40) == 0) {
    psVar7 = (short *)param_2[0x1e];
    psVar6 = (short *)param_1[0x1e];
    iVar13 = (int)*psVar6 + param_1[0xd];
    iVar12 = ((int)*psVar7 + param_2[0xd]) - iVar13;
    iVar15 = (int)psVar6[2] + param_1[0xf];
    iVar10 = ((int)psVar7[2] + param_2[0xf]) - iVar15;
    iVar5 = (uint)(ushort)psVar6[3] + (uint)(ushort)psVar7[3];
    iVar9 = (uint)(ushort)psVar6[5] + (uint)(ushort)psVar7[5];
    uVar2 = 0;
    if (((uint)(iVar12 + iVar5) <= (uint)(iVar5 * 2)) &&
       (uVar2 = 0, (uint)(iVar10 + iVar9) <= (uint)(iVar9 * 2))) {
      iVar8 = (int)psVar7[1];
      iVar14 = (int)psVar6[1] + param_1[0xe];
      iVar11 = (iVar8 + param_2[0xe]) - iVar14;
      iVar4 = (uint)(ushort)psVar6[4] + (uint)(ushort)psVar7[4];
      bVar1 = (uint)(iVar11 + iVar4) <= (uint)(iVar4 * 2);
      uVar2 = 0;
      if (bVar1) {
        iVar16 = iVar12 * iVar9;
        if ((uint)(((*(short *)((int)param_2 + 0x42) + iVar8) -
                   ((int)*(short *)((int)param_1 + 0x42) + (int)psVar6[1])) + iVar4) <=
            (uint)(iVar4 * 2) == bVar1 || !bVar1) {
          iVar14 = iVar10 * iVar5;
          if (iVar16 < 0) {
            iVar16 = -iVar16;
          }
          if (iVar14 < 0) {
            iVar14 = -iVar14;
          }
          if (iVar14 < iVar16) {
            iVar13 = iVar13 - *psVar6;
            if (iVar12 < 0) {
              param_2[0xd] = (iVar13 + -1) - iVar5;
            }
            else {
              param_2[0xd] = iVar13 + 1 + iVar5;
            }
          }
          else {
            iVar15 = iVar15 - psVar6[2];
            if (iVar10 < 0) {
              uVar3 = (iVar15 + -1) - iVar9;
            }
            else {
              uVar3 = iVar15 + 1 + iVar9;
            }
            param_2[0xf] = uVar3;
          }
        }
        else {
          iVar14 = iVar14 - iVar8;
          if (iVar11 < 0) {
            param_2[0xe] = (iVar14 + -1) - iVar4;
          }
          else {
            param_2[0xe] = iVar14 + 1 + iVar4;
          }
        }
        uVar2 = 1;
      }
    }
  }
  return uVar2;
}
