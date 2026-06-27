/* FUN_80034d0c @ 0x80034d0c  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80034d0c(uint *param_1,uint *param_2)

{
  bool bVar1;
  long lVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  uint *puVar11;
  int iVar12;
  int iVar13;
  uint *puVar14;
  int iVar15;
  
  uVar4 = *param_2;
  uVar3 = 0;
  if (((((*param_1 | uVar4) & 2) == 0) && (uVar3 = 0, (*param_1 & uVar4 & 0x1000) == 0)) &&
     (uVar3 = 0, (uVar4 & 4) == 0)) {
    DAT_800c3afc = param_1[0x7a];
    puVar14 = param_1 + 0x21;
    DAT_800c3af8 = 0;
    while( true ) {
      DAT_800c3afc = DAT_800c3afc - 1;
      puVar11 = param_2 + 0x21;
      uVar3 = DAT_800c3af8;
      if ((int)DAT_800c3afc < 0) break;
      uVar4 = (uint)*(ushort *)((int)puVar14 + 0x16);
      DAT_800c3b00 = param_2[0x7a];
      while (DAT_800c3b00 = DAT_800c3b00 - 1, -1 < (int)DAT_800c3b00) {
        iVar10 = uVar4 + *(ushort *)((int)puVar11 + 0x16);
        iVar12 = *puVar11 - *puVar14;
        if ((uint)(iVar12 + iVar10) <= (uint)(iVar10 * 2)) {
          iVar13 = puVar11[2] - puVar14[2];
          if ((uint)(iVar13 + iVar10) <= (uint)(iVar10 * 2)) {
            lVar2 = SquareRoot0(iVar12 * iVar12 + iVar13 * iVar13);
            iVar10 = iVar10 - lVar2;
            if (0 < iVar10) {
              iVar5 = puVar11[1] - puVar14[1];
              iVar8 = (uint)*(ushort *)((int)puVar11 + 0x1a) +
                      (uint)*(ushort *)((int)puVar14 + 0x1a);
              if ((-iVar8 < iVar5) && (iVar5 < iVar8)) {
                iVar9 = lVar2 + 1;
                iVar5 = (iVar12 * iVar10) / iVar9;
                if (iVar9 == 0) {
                  trap(0x1c00);
                }
                if ((iVar9 == -1) && (iVar12 * iVar10 == -0x80000000)) {
                  trap(0x1800);
                }
                iVar6 = ((int)*(short *)((int)param_2 + 0x46) + (int)(short)puVar11[5]) - puVar14[1]
                ;
                iVar15 = (iVar13 * iVar10) / iVar9;
                if (iVar9 == 0) {
                  trap(0x1c00);
                }
                if ((iVar9 == -1) && (iVar13 * iVar10 == -0x80000000)) {
                  trap(0x1800);
                }
                if ((iVar6 <= -iVar8) || (iVar8 <= iVar6)) {
                  uVar7 = param_1[0xe];
                  if ((((int)(short)param_2[0x11] < (int)uVar7) && ((int)uVar7 < (int)param_2[0xe]))
                     || (((int)uVar7 < (int)(short)param_2[0x11] && ((int)param_2[0xe] < (int)uVar7)
                         ))) {
                    iVar8 = -(iVar12 * iVar10) / iVar9;
                    if (iVar9 == 0) {
                      trap(0x1c00);
                    }
                    if ((iVar9 == -1) && (-(iVar12 * iVar10) == -0x80000000)) {
                      trap(0x1800);
                    }
                    iVar5 = uVar4 * 2;
                    if (iVar8 < 1) {
                      iVar5 = uVar4 * -2;
                    }
                    iVar5 = iVar5 - iVar8;
                  }
                  uVar7 = param_1[0x10];
                  if ((((int)(short)param_2[0x12] < (int)uVar7) && ((int)uVar7 < (int)param_2[0x10])
                      ) || (((int)uVar7 < (int)(short)param_2[0x12] &&
                            ((int)param_2[0x10] < (int)uVar7)))) {
                    iVar12 = -(iVar13 * iVar10) / iVar9;
                    if (iVar9 == 0) {
                      trap(0x1c00);
                    }
                    if ((iVar9 == -1) && (-(iVar13 * iVar10) == -0x80000000)) {
                      trap(0x1800);
                    }
                    iVar15 = uVar4 * 2;
                    if (iVar12 < 1) {
                      iVar15 = uVar4 * -2;
                    }
                    iVar15 = iVar15 - iVar12;
                  }
                }
                if ((*param_1 & 0x100000) != 0) {
                  iVar10 = iVar5;
                  if (iVar5 < 0) {
                    iVar10 = -iVar5;
                  }
                  if (100 < iVar10) {
                    if (iVar5 < 0) {
                      iVar5 = -100;
                    }
                    else {
                      iVar15 = 100;
                    }
                  }
                  iVar10 = iVar15;
                  if (iVar15 < 0) {
                    iVar10 = -iVar15;
                  }
                  if ((100 < iVar10) && (bVar1 = iVar15 < 0, iVar15 = 100, bVar1)) {
                    iVar15 = -100;
                  }
                }
                DAT_800c3af8 = 1;
                param_2[0xe] = param_2[0xe] + iVar5;
                param_2[0x10] = param_2[0x10] + iVar15;
                *puVar11 = *puVar11 + iVar5;
                puVar11[2] = puVar11[2] + iVar15;
                *(char *)((int)param_2 + 0xd) = (char)param_1[3];
              }
            }
          }
        }
        puVar11 = puVar11 + 8;
      }
      puVar14 = puVar14 + 8;
    }
  }
  return uVar3;
}


