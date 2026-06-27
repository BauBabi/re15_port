/* FUN_80037f60 @ 0x80037f60  (Ghidra headless, raw MIPS overlay) */

void FUN_80037f60(uint *param_1,uint *param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint uVar4;
  byte bVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  ushort *puVar14;
  uint *puVar15;
  uint *puVar16;
  uint uVar17;
  int iVar18;
  uint uVar19;
  uint uVar20;
  undefined1 auStack_54 [20];
  uint *local_40;
  uint local_38;
  int local_34;
  
  local_40 = param_2;
  uVar17 = (uint)&stack0xffffffab & 3;
  puVar2 = (uint *)(&stack0xffffffab + -uVar17);
  *puVar2 = *puVar2 & -1 << (uVar17 + 1) * 8 | DAT_80010b9c >> (3 - uVar17) * 8;
  puVar1 = auStack_54 + 3;
  uVar17 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar17) =
       *(uint *)(puVar1 + -uVar17) & -1 << (uVar17 + 1) * 8 | DAT_80010ba0 >> (3 - uVar17) * 8;
  auStack_54._0_4_ = DAT_80010ba0;
  if (((*param_1 & 0x40) == 0) && ((*local_40 & 2) == 0)) {
    puVar16 = param_1 + 0x21;
    puVar3 = (uint *)FUN_8003873c(param_1,puVar16,param_3,param_4,
                                  CONCAT22((short)(DAT_80010b9c >> 0x10),
                                           (short)DAT_80010b9c + *(short *)((int)param_1 + 0x9a)));
    puVar2 = local_40;
    param_1[0x79] = (uint)puVar3;
    puVar15 = local_40 + 0x21;
    if (puVar3 == local_40) {
      iVar18 = -(uint)(ushort)((ushort)param_1[0x24] >> 1);
      iVar13 = iVar18 + (uint)(ushort)local_40[0x24];
      iVar18 = iVar18 + (uint)*(ushort *)((int)local_40 + 0x92);
      uVar20 = local_40[0x21] - param_1[0x21];
      uVar19 = local_40[0x23] - param_1[0x23];
      uVar17 = (uint)((uint)(iVar13 * 2) < uVar20 + iVar13);
      if ((uint)(iVar18 * 2) < uVar19 + iVar18) {
        uVar17 = uVar17 | 2;
      }
      uVar12 = (uint)((uint)(iVar13 * 2) <
                     (local_40[0x21] -
                     ((int)*(short *)((int)param_1 + 0x4a) + (int)(short)param_1[0x25])) + iVar13);
      if ((uint)(iVar18 * 2) <
          (local_40[0x23] -
          ((int)*(short *)((int)param_1 + 0x4e) + (int)*(short *)((int)param_1 + 0x96))) + iVar18) {
        uVar12 = uVar12 | 2;
      }
      uVar4 = uVar17 & 2;
      if ((uVar17 & 1) != 0) {
        puVar3 = &DAT_800d0324;
        uVar4 = uVar17 & 2;
        if (&DAT_800d0324 < DAT_800d4224) {
          puVar14 = &DAT_800d03b4;
          do {
            if ((((puVar3 != local_40) && ((*puVar3 & 0x101) == 0x101)) && ((*puVar3 & 2) == 0)) &&
               (puVar2[0x22] - (uint)*(ushort *)((int)puVar2 + 0x9e) ==
                *(int *)(puVar14 + -4) - (uint)puVar14[7])) {
              uVar4 = (uint)puVar14[1];
              iVar10 = *(int *)(puVar14 + -2);
              uVar9 = puVar2[0x23];
              iVar6 = (*(ushort *)((int)puVar2 + 0x92) + uVar4) - (uint)(ushort)param_1[0x24];
              if ((iVar10 - uVar9) + iVar6 < (uint)(iVar6 * 2)) {
                uVar11 = param_1[0x23];
                if ((((iVar10 - uVar11) + uVar4 <= uVar4 << 1) &&
                    (iVar6 = (uint)(ushort)puVar2[0x24] + (uint)*puVar14 + 200,
                    (*(int *)(puVar14 + -6) - *puVar15) + iVar6 <= (uint)(iVar6 * 2))) &&
                   (0 < (int)(*puVar15 - *(int *)(puVar14 + -6) ^ uVar20))) {
                  iVar6 = -(uint)(*(ushort *)((int)param_1 + 0x92) >> 1);
                  iVar7 = iVar6 + (uint)*(ushort *)((int)puVar2 + 0x92);
                  iVar6 = iVar6 + uVar4;
                  uVar4 = iVar7 * 2;
                  uVar8 = iVar6 * 2;
                  bVar5 = (uVar9 - uVar11) + iVar7 < uVar4;
                  if ((iVar10 - uVar11) + iVar6 < uVar8) {
                    bVar5 = bVar5 | 2;
                  }
                  if (bVar5 == 3) {
                    iVar18 = (int)*(short *)((int)param_1 + 0x4e) +
                             (int)*(short *)((int)param_1 + 0x96);
                    bVar5 = uVar4 < (uVar9 - iVar18) + iVar7;
                    if (uVar8 < (uint)((iVar10 - iVar18) + iVar6)) {
                      bVar5 = bVar5 | 2;
                    }
                    if (bVar5 == 0) {
                      return;
                    }
                    goto LAB_800385ec;
                  }
                  if ((uVar12 == 0) && ((bVar5 & 1) != 0)) goto LAB_800385ec;
                  uVar17 = 2;
                  if (bVar5 == 1) {
                    FUN_800386d0(param_1,puVar3 + 0x21,puVar3[0x23] - uVar11,iVar18);
                    return;
                  }
                  iVar10 = (int)*(short *)((int)param_1 + 0x4e) +
                           (int)*(short *)((int)param_1 + 0x96);
                  if (((uVar9 - iVar10) + iVar7 < uVar4) &&
                     ((puVar3[0x23] - iVar10) + iVar6 < uVar8)) goto LAB_800385b0;
                  if ((int)(uVar19 ^ uVar9 - puVar3[0x23]) < 0) {
                    local_38 = uVar12;
                    local_34 = iVar13;
                    FUN_80038664(param_1,puVar15,uVar20,iVar13);
                    uVar12 = local_38;
                    iVar13 = local_34;
                  }
                }
              }
            }
            puVar3 = puVar3 + 0x7e;
            puVar14 = puVar14 + 0xfc;
          } while (puVar3 < DAT_800d4224);
          uVar4 = uVar17 & 2;
        }
      }
      uVar9 = uVar12 ^ uVar17;
      if (uVar4 != 0) {
        puVar3 = &DAT_800d0324;
        uVar9 = uVar12 ^ uVar17;
        if (&DAT_800d0324 < DAT_800d4224) {
          puVar14 = &DAT_800d03b6;
          do {
            if ((((puVar3 != local_40) && ((*puVar3 & 0x101) == 0x101)) && ((*puVar3 & 2) == 0)) &&
               (puVar2[0x22] - (uint)*(ushort *)((int)puVar2 + 0x9e) ==
                *(int *)(puVar14 + -5) - (uint)puVar14[6])) {
              uVar4 = (uint)puVar14[-1];
              iVar10 = *(int *)(puVar14 + -7);
              uVar9 = *puVar15;
              iVar6 = ((ushort)puVar2[0x24] + uVar4) - (uint)(ushort)param_1[0x24];
              if ((iVar10 - uVar9) + iVar6 < (uint)(iVar6 * 2)) {
                uVar11 = *puVar16;
                if ((((iVar10 - uVar11) + uVar4 <= uVar4 << 1) &&
                    (iVar6 = (uint)*(ushort *)((int)puVar2 + 0x92) + (uint)*puVar14 + 200,
                    (*(int *)(puVar14 + -3) - puVar2[0x23]) + iVar6 <= (uint)(iVar6 * 2))) &&
                   (0 < (int)(puVar2[0x23] - *(int *)(puVar14 + -3) ^ uVar19))) {
                  iVar6 = -(uint)(ushort)((ushort)param_1[0x24] >> 1);
                  iVar7 = iVar6 + (uint)(ushort)puVar2[0x24];
                  iVar6 = iVar6 + uVar4;
                  uVar4 = iVar7 * 2;
                  uVar8 = iVar6 * 2;
                  bVar5 = (uVar9 - uVar11) + iVar7 < uVar4;
                  if ((iVar10 - uVar11) + iVar6 < uVar8) {
                    bVar5 = bVar5 | 2;
                  }
                  if (bVar5 == 3) {
                    iVar13 = (int)*(short *)((int)param_1 + 0x4a) + (int)(short)param_1[0x25];
                    bVar5 = (uVar9 - iVar13) + iVar7 < uVar4;
                    if ((uint)((iVar10 - iVar13) + iVar6) < uVar8) {
                      bVar5 = bVar5 | 2;
                    }
                    if (bVar5 == 3) {
                      return;
                    }
LAB_800385b0:
                    FUN_800386d0(param_1,puVar15,uVar19,iVar18);
                    return;
                  }
                  uVar17 = 1;
                  if ((uVar12 == 0) && ((bVar5 & 1) != 0)) goto LAB_800385b0;
                  if (bVar5 == 1) {
                    FUN_80038664(param_1,puVar3 + 0x21,puVar3[0x21] - uVar11,iVar13);
                    return;
                  }
                  iVar10 = (int)*(short *)((int)param_1 + 0x4a) + (int)(short)param_1[0x25];
                  if (((uVar9 - iVar10) + iVar7 < uVar4) &&
                     ((puVar3[0x21] - iVar10) + iVar6 < uVar8)) {
LAB_800385ec:
                    FUN_80038664(param_1,puVar15,uVar20,iVar13);
                    return;
                  }
                  if ((int)(uVar20 ^ uVar9 - puVar3[0x21]) < 0) {
                    local_38 = uVar12;
                    local_34 = iVar13;
                    FUN_800386d0(param_1,puVar15,uVar19,iVar18);
                    uVar12 = local_38;
                    iVar13 = local_34;
                  }
                }
              }
            }
            puVar3 = puVar3 + 0x7e;
            puVar14 = puVar14 + 0xfc;
          } while (puVar3 < DAT_800d4224);
          uVar9 = uVar12 ^ uVar17;
        }
      }
      if ((uVar17 & uVar9 & 1) != 0) {
        uVar12 = param_1[0xe];
        if (-1 < (int)uVar20) {
          iVar13 = -iVar13;
        }
        param_1[0xe] = (*puVar15 - (int)(short)param_1[0x25]) + iVar13;
        *puVar16 = *puVar16 + (param_1[0xe] - uVar12);
      }
      if ((uVar17 & uVar9 & 2) != 0) {
        uVar17 = param_1[0x10];
        if (-1 < (int)uVar19) {
          iVar18 = -iVar18;
        }
        param_1[0x10] = (puVar2[0x23] - (int)*(short *)((int)param_1 + 0x96)) + iVar18;
        param_1[0x23] = param_1[0x23] + (param_1[0x10] - uVar17);
      }
      uVar17 = FUN_8003873c(param_1,puVar16);
      param_1[0x79] = uVar17;
    }
  }
  return;
}


