/* FUN_8001468c @ 0x8001468c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001468c(int param_1,int param_2,int *param_3)

{
  undefined1 *puVar1;
  ushort uVar2;
  int iVar3;
  uint *puVar4;
  ushort uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  undefined4 uVar9;
  uint uVar10;
  uint uVar11;
  byte *pbVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  uint *r0;
  SVECTOR *pSVar16;
  short *psVar17;
  short *psVar18;
  SVECTOR *pSVar19;
  SVECTOR *r1;
  uint *unaff_s4;
  int iVar20;
  int iVar21;
  SVECTOR *r0_00;
  undefined1 *puVar22;
  short *psVar23;
  undefined1 auStack_70 [72];
  
  puVar22 = auStack_70;
  uVar5 = *(ushort *)(param_2 + 0x144);
  if (((uVar5 & 0x400) != 0) && (*(short *)(DAT_800c3a80 + 0x22e) == *(short *)(param_2 + 0x10e))) {
    uVar2 = uVar5 | 0x80;
    if ((uVar5 & 0xc0) == 0xc0) {
      uVar2 = uVar5 & 0xff7f;
    }
    *(ushort *)(param_2 + 0x144) = uVar2;
  }
  pbVar12 = &DAT_800ce5e0;
  psVar23 = (short *)param_3[4];
  iVar13 = param_3[5];
  uVar10 = *(uint *)(param_1 + 8);
  iVar21 = *param_3;
  iVar14 = param_3[2];
  r0 = (uint *)(uVar10 + (uint)DAT_800ce5e0 * 0x28);
  *(byte *)(param_1 + 0x6f) = (byte)((ushort)*(undefined2 *)(param_2 + 0x144) >> 0xd) & 2 | 0x34;
  gte_ldrgb((CVECTOR *)(param_1 + 0x6c));
  uVar15 = 0xffffff;
  r0_00 = (SVECTOR *)(iVar21 + psVar23[1] * 8);
  uVar6 = psVar23[3] * 8;
  pSVar19 = (SVECTOR *)(iVar21 + uVar6);
  pSVar16 = (SVECTOR *)(iVar21 + psVar23[5] * 8);
  if (iVar13 != 0) {
    pbVar12 = pbVar12 + 0x6214;
    psVar17 = psVar23 + 5;
    do {
      if ((*(ushort *)(param_2 + 0x144) & 0x2000) != 0) {
        pSVar19->vz = (short)((uint)((int)pSVar19->vz * (int)*(short *)pbVar12) >> 8);
        pSVar16->vz = (short)((uint)((int)pSVar16->vz * (int)*(short *)pbVar12) >> 8);
        uVar6 = (int)r0_00->vz * (int)*(short *)pbVar12;
        r0_00->vz = (short)(uVar6 >> 8);
      }
      gte_ldv3(r0_00,pSVar19,pSVar16);
      iVar20 = DAT_800c3a80;
      gte_rtpt();
      iVar13 = iVar13 + -1;
      if ((*(ushort *)(param_2 + 0x144) & 0x20) != 0) {
        puVar1 = (undefined1 *)((int)&r0_00->vy + 1);
        uVar8 = (uint)puVar1 & 3;
        uVar7 = (uint)r0_00 & 3;
        uVar7 = (*(int *)(puVar1 + -uVar8) << (3 - uVar8) * 8 |
                uVar6 & 0xffffffffU >> (uVar8 + 1) * 8) & -1 << (4 - uVar7) * 8 |
                *(uint *)((int)r0_00 - uVar7) >> uVar7 * 8;
        puVar1 = (undefined1 *)((int)&r0_00->pad + 1);
        uVar6 = (uint)puVar1 & 3;
        uVar8 = (uint)&r0_00->vz & 3;
        uVar11 = (*(int *)(puVar1 + -uVar6) << (3 - uVar6) * 8 |
                 uVar10 & 0xffffffffU >> (uVar6 + 1) * 8) & -1 << (4 - uVar8) * 8 |
                 *(uint *)((int)&r0_00->vz - uVar8) >> uVar8 * 8;
        uVar6 = DAT_800c3a80 + 0x233U & 3;
        puVar4 = (uint *)((DAT_800c3a80 + 0x233U) - uVar6);
        *puVar4 = *puVar4 & -1 << (uVar6 + 1) * 8 | uVar7 >> (3 - uVar6) * 8;
        uVar6 = iVar20 + 0x230U & 3;
        puVar4 = (uint *)((iVar20 + 0x230U) - uVar6);
        *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar6) * 8 | uVar7 << uVar6 * 8;
        uVar6 = iVar20 + 0x237U & 3;
        puVar4 = (uint *)((iVar20 + 0x237U) - uVar6);
        *puVar4 = *puVar4 & -1 << (uVar6 + 1) * 8 | uVar11 >> (3 - uVar6) * 8;
        uVar6 = iVar20 + 0x234U & 3;
        puVar4 = (uint *)((iVar20 + 0x234U) - uVar6);
        *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar6) * 8 | uVar11 << uVar6 * 8;
        iVar20 = DAT_800c3a80;
        puVar1 = (undefined1 *)((int)&pSVar19->vy + 1);
        uVar6 = (uint)puVar1 & 3;
        uVar10 = (uint)pSVar19 & 3;
        uVar8 = (*(int *)(puVar1 + -uVar6) << (3 - uVar6) * 8 |
                uVar7 & 0xffffffffU >> (uVar6 + 1) * 8) & -1 << (4 - uVar10) * 8 |
                *(uint *)((int)pSVar19 - uVar10) >> uVar10 * 8;
        puVar1 = (undefined1 *)((int)&pSVar19->pad + 1);
        uVar6 = (uint)puVar1 & 3;
        uVar10 = (uint)&pSVar19->vz & 3;
        uVar7 = (*(int *)(puVar1 + -uVar6) << (3 - uVar6) * 8 |
                uVar11 & 0xffffffffU >> (uVar6 + 1) * 8) & -1 << (4 - uVar10) * 8 |
                *(uint *)((int)&pSVar19->vz - uVar10) >> uVar10 * 8;
        uVar6 = DAT_800c3a80 + 0x23bU & 3;
        puVar4 = (uint *)((DAT_800c3a80 + 0x23bU) - uVar6);
        *puVar4 = *puVar4 & -1 << (uVar6 + 1) * 8 | uVar8 >> (3 - uVar6) * 8;
        uVar6 = iVar20 + 0x238U & 3;
        puVar4 = (uint *)((iVar20 + 0x238U) - uVar6);
        *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar6) * 8 | uVar8 << uVar6 * 8;
        uVar6 = iVar20 + 0x23fU & 3;
        puVar4 = (uint *)((iVar20 + 0x23fU) - uVar6);
        *puVar4 = *puVar4 & -1 << (uVar6 + 1) * 8 | uVar7 >> (3 - uVar6) * 8;
        uVar6 = iVar20 + 0x23cU & 3;
        puVar4 = (uint *)((iVar20 + 0x23cU) - uVar6);
        *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar6) * 8 | uVar7 << uVar6 * 8;
        iVar20 = DAT_800c3a80;
        puVar1 = (undefined1 *)((int)&pSVar16->vy + 1);
        uVar6 = (uint)puVar1 & 3;
        uVar10 = (uint)pSVar16 & 3;
        uVar8 = (*(int *)(puVar1 + -uVar6) << (3 - uVar6) * 8 |
                uVar8 & 0xffffffffU >> (uVar6 + 1) * 8) & -1 << (4 - uVar10) * 8 |
                *(uint *)((int)pSVar16 - uVar10) >> uVar10 * 8;
        puVar1 = (undefined1 *)((int)&pSVar16->pad + 1);
        uVar6 = (uint)puVar1 & 3;
        uVar10 = (uint)&pSVar16->vz & 3;
        uVar10 = (*(int *)(puVar1 + -uVar6) << (3 - uVar6) * 8 |
                 uVar7 & 0xffffffffU >> (uVar6 + 1) * 8) & -1 << (4 - uVar10) * 8 |
                 *(uint *)((int)&pSVar16->vz - uVar10) >> uVar10 * 8;
        uVar6 = DAT_800c3a80 + 0x243U & 3;
        puVar4 = (uint *)((DAT_800c3a80 + 0x243U) - uVar6);
        *puVar4 = *puVar4 & -1 << (uVar6 + 1) * 8 | uVar8 >> (3 - uVar6) * 8;
        uVar6 = iVar20 + 0x240U & 3;
        puVar4 = (uint *)((iVar20 + 0x240U) - uVar6);
        *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar6) * 8 | uVar8 << uVar6 * 8;
        uVar6 = iVar20 + 0x247U & 3;
        puVar4 = (uint *)((iVar20 + 0x247U) - uVar6);
        *puVar4 = *puVar4 & -1 << (uVar6 + 1) * 8 | uVar10 >> (3 - uVar6) * 8;
        uVar6 = iVar20 + 0x244U & 3;
        puVar4 = (uint *)((iVar20 + 0x244U) - uVar6);
        *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar6) * 8 | uVar10 << uVar6 * 8;
      }
      r1 = (SVECTOR *)(iVar14 + psVar17[-3] * 8);
      pSVar19 = (SVECTOR *)(iVar14 + psVar17[-1] * 8);
      pSVar16 = (SVECTOR *)(iVar14 + *psVar23 * 8);
      gte_nclip();
      psVar18 = psVar17 + 6;
      psVar23 = psVar23 + 6;
      r0_00 = (SVECTOR *)(iVar21 + psVar17[2] * 8);
      gte_stopz((long *)(puVar22 + 0x30));
      if (*(int *)(puVar22 + 0x30) < 0) {
        *(short *)(DAT_800c3a80 + 0x22c) = *(short *)(DAT_800c3a80 + 0x22c) + 1;
        uVar6 = psVar18[-2] * 8;
        pSVar19 = (SVECTOR *)(iVar21 + uVar6);
        pSVar16 = (SVECTOR *)(iVar21 + *psVar18 * 8);
      }
      else {
        gte_ldv3(pSVar16,r1,pSVar19);
        gte_ncct();
        pSVar19 = (SVECTOR *)(iVar21 + psVar18[-2] * 8);
        pSVar16 = (SVECTOR *)(iVar21 + *psVar18 * 8);
        gte_stsxy3_gt3(r0);
        gte_strgb3_gt3(r0);
        gte_avsz3();
        gte_stotz((long *)(puVar22 + 0x34));
        uVar10 = *(uint *)(puVar22 + 0x34);
        if (uVar10 >> 6 != 0) {
          uVar5 = *(ushort *)(param_2 + 0x144) & 0xc0;
          iVar20 = DAT_800ce22c;
          if (uVar5 == 0x80) {
            iVar3 = (uVar10 >> 7) * 4 + 0x7fc;
LAB_80014a24:
            unaff_s4 = (uint *)(iVar20 + iVar3);
          }
          else if (uVar5 < 0x81) {
            if (uVar5 == 0x40) {
              iVar3 = (uVar10 >> 0xc) << 2;
              iVar20 = DAT_800ce2b0;
              goto LAB_80014a24;
            }
          }
          else if (uVar5 == 0xc0) {
            iVar3 = (uVar10 >> 7) << 2;
            goto LAB_80014a24;
          }
          if ((*(ushort *)(param_2 + 0x144) & 0x20) == 0) {
            *(uint *)(puVar22 + 0x34) = *unaff_s4;
            *unaff_s4 = (uint)r0 & uVar15;
            *r0 = *(uint *)(puVar22 + 0x34) & uVar15 | 0x9000000;
          }
          else {
            *(uint **)(puVar22 + 0x10) = r0 + 6;
            *(uint **)(puVar22 + 0x14) = r0 + 9;
            puVar4 = r0 + 1;
            *(uint **)(puVar22 + 0x18) = puVar4;
            *(uint **)(puVar22 + 0x1c) = puVar4;
            *(uint **)(puVar22 + 0x20) = puVar4;
            uVar10 = DAT_800c3a80 + 0x230;
            uVar9 = *(undefined4 *)(DAT_800c3a80 + 8);
            *(uint **)(puVar22 + 0x28) = unaff_s4;
            *(int *)(puVar22 + 0x2c) = DAT_800c3a80 + 0xc;
            *(int *)(puVar22 + 0x38) = iVar13;
            *(int *)(puVar22 + 0x3c) = iVar14;
            *(uint *)(puVar22 + 0x40) = uVar15;
            *(byte **)(puVar22 + 0x44) = pbVar12;
            *(undefined4 *)(puVar22 + 0x24) = uVar9;
            uVar9 = DivideGT3(uVar10,DAT_800c3a80 + 0x238,DAT_800c3a80 + 0x240,r0 + 3);
            *(undefined4 *)(DAT_800c3a80 + 8) = uVar9;
            iVar13 = *(int *)(puVar22 + 0x38);
            iVar14 = *(int *)(puVar22 + 0x3c);
            uVar15 = *(uint *)(puVar22 + 0x40);
            pbVar12 = *(byte **)(puVar22 + 0x44);
          }
        }
        uVar6 = DAT_800c3a80;
        *(short *)(DAT_800c3a80 + 0x22c) = *(short *)(DAT_800c3a80 + 0x22c) + 1;
      }
      r0 = r0 + 0x14;
      psVar17 = psVar18;
    } while (iVar13 != 0);
  }
  return;
}


