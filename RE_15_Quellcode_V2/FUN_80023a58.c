void FUN_80023a58(int *param_1,uint *param_2,CVECTOR *param_3,int param_4)

{
  short sVar1;
  short sVar2;
  short sVar3;
  uint uVar4;
  uint uVar5;
  short *psVar6;
  VECTOR *r0;
  SVECTOR *r0_00;
  SVECTOR *pSVar7;
  SVECTOR *pSVar8;
  int iVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  int iVar13;
  int iVar14;
  MATRIX *r0_01;
  short *psVar15;
  int iVar16;
  undefined *puVar17;
  short *psVar18;
  SVECTOR *r0_02;
  undefined4 *puVar19;
  uint uVar20;
  undefined1 local_88 [96];
  
  puVar19 = (undefined4 *)local_88;
  psVar18 = (short *)param_1[4];
  iVar13 = param_1[5];
  iVar9 = *param_1;
  iVar14 = param_1[2];
  uVar4 = (uint)DAT_800aca34;
                    /* Possible PsyQ macro: setPolyGT3() */
  param_3->cd = '4';
  puVar17 = &DAT_800aa6d8 + uVar4 * 0x1000;
  gte_ldrgb(param_3);
  uVar4 = 0xffffff;
  uVar20 = 0x9000000;
  sVar1 = psVar18[1];
  sVar2 = psVar18[3];
  sVar3 = psVar18[5];
  psVar15 = (short *)(param_4 + 0x94);
  *puVar19 = 0;
  puVar19[2] = 0;
  r0_02 = (SVECTOR *)(iVar9 + sVar1 * 8);
  pSVar8 = (SVECTOR *)(iVar9 + sVar2 * 8);
  pSVar7 = (SVECTOR *)(iVar9 + sVar3 * 8);
  r0_01 = (MATRIX *)(puVar19 + 4);
  *(short *)(param_4 + 0x94) = (short)*(char *)(param_4 + 0x98) + *(short *)(param_4 + 0x94);
  uVar10 = gte_stR11R12();
  uVar11 = gte_stR13R21();
  *(undefined4 *)r0_01->m[0] = uVar10;
  puVar19[5] = uVar11;
  uVar10 = gte_stR22R23();
  uVar11 = gte_stR31R32();
  uVar12 = gte_stR33();
  puVar19[6] = uVar10;
  puVar19[7] = uVar11;
  puVar19[8] = uVar12;
  uVar10 = gte_stTRX();
  uVar11 = gte_stTRY();
  uVar12 = gte_stTRZ();
  puVar19[9] = uVar10;
  puVar19[10] = uVar11;
  puVar19[0xb] = uVar12;
  r0 = (VECTOR *)(puVar19 + 0x11);
  if (iVar13 != 0) {
    iVar16 = (int)*(short *)(param_4 + 0x58);
    puVar19[0x16] = -iVar16;
    psVar6 = psVar18 + 5;
    do {
      *(int *)((int)puVar19 + 4) = (int)*psVar15;
      iVar13 = iVar13 + -1;
      if (0 < iVar16 + r0_02->vy + (int)*psVar15) {
        *(int *)((int)puVar19 + 4) = *(int *)((int)puVar19 + 0x58) - (int)r0_02->vy;
      }
      gte_SetRotMatrix((MATRIX *)&DAT_800b5288);
      gte_ldVXY0(CONCAT22(*(undefined2 *)((int)puVar19 + 4),*(undefined2 *)puVar19));
      gte_ldVZ0((undefined2 *)((int)puVar19 + 8));
      gte_rtv0();
      r0_00 = (SVECTOR *)(iVar14 + *psVar18 * 8);
      gte_stlvnl(r0);
      gte_SetRotMatrix(r0_01);
      gte_ldv3(r0_02,pSVar8,pSVar7);
      gte_rtpt();
      pSVar8 = (SVECTOR *)(iVar14 + psVar6[-3] * 8);
      pSVar7 = (SVECTOR *)(iVar14 + psVar6[-1] * 8);
      r0->vx = r0->vx + *(int *)((int)puVar19 + 0x24);
      gte_nclip();
      psVar6 = psVar6 + 6;
      r0->vy = r0->vy + *(int *)((int)puVar19 + 0x28);
      psVar18 = psVar18 + 6;
      r0->vz = r0->vz + *(int *)((int)puVar19 + 0x2c);
      gte_stsxy3_gt3(param_2);
      gte_stopz((long *)((int)puVar19 + 0x50));
      gte_SetTransMatrix((MATRIX *)((int)puVar19 + 0x30));
      gte_ldv0(r0_02);
      gte_rtps();
      r0_02 = (SVECTOR *)(iVar9 + psVar6[-4] * 8);
      gte_stsxy((long *)(param_2 + 2));
      gte_SetTransMatrix(r0_01);
      if (*(int *)((int)puVar19 + 0x50) < 0) {
        pSVar8 = (SVECTOR *)(iVar9 + psVar6[-2] * 8);
        pSVar7 = (SVECTOR *)(iVar9 + *psVar6 * 8);
      }
      else {
        gte_ldv3(r0_00,pSVar8,pSVar7);
        gte_ncct();
        pSVar8 = (SVECTOR *)(iVar9 + psVar6[-2] * 8);
        pSVar7 = (SVECTOR *)(iVar9 + *psVar6 * 8);
        gte_strgb3_gt3(param_2);
        gte_avsz3();
        gte_stotz((long *)((int)puVar19 + 0x54));
        uVar5 = *(uint *)((int)puVar19 + 0x54);
        if (uVar5 >> 6 != 0) {
          *(uint *)((int)puVar19 + 0x54) = *(uint *)(puVar17 + (uVar5 >> 4) * 4);
          *(uint *)(puVar17 + (uVar5 >> 4) * 4) = (uint)param_2 & uVar4;
          *param_2 = uVar4 & *(uint *)((int)puVar19 + 0x54) | uVar20;
        }
      }
      param_2 = param_2 + 0x14;
    } while (iVar13 != 0);
  }
  return;
}
