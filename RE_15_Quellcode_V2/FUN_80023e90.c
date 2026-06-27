void FUN_80023e90(int *param_1,uint *param_2,CVECTOR *param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  uint *r0;
  short *psVar3;
  short *psVar4;
  CVECTOR *r0_00;
  int iVar5;
  VECTOR *r0_01;
  SVECTOR *pSVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  int iVar11;
  MATRIX *r0_02;
  short *psVar12;
  SVECTOR *r0_03;
  int iVar13;
  SVECTOR *pSVar14;
  SVECTOR *pSVar15;
  short *psVar16;
  undefined4 *puVar17;
  undefined *puVar18;
  undefined1 local_88 [96];
  
  puVar17 = (undefined4 *)local_88;
  psVar16 = (short *)param_1[4];
  iVar11 = param_1[5];
  iVar5 = *param_1;
  iVar10 = param_1[2];
  uVar2 = (uint)DAT_800aca34;
                    /* Possible PsyQ macro: setPolyGT4() */
  param_3->cd = '<';
  puVar18 = &DAT_800aa6d8 + uVar2 * 0x1000;
  gte_ldrgb(param_3);
  uVar2 = 0xffffff;
  r0_02 = (MATRIX *)(puVar17 + 4);
  r0_03 = (SVECTOR *)(iVar5 + psVar16[1] * 8);
  pSVar14 = (SVECTOR *)(iVar5 + psVar16[3] * 8);
  pSVar6 = (SVECTOR *)(iVar5 + psVar16[5] * 8);
  uVar7 = gte_stR11R12();
  uVar8 = gte_stR13R21();
  *(undefined4 *)r0_02->m[0] = uVar7;
  puVar17[5] = uVar8;
  uVar7 = gte_stR22R23();
  uVar8 = gte_stR31R32();
  uVar9 = gte_stR33();
  puVar17[6] = uVar7;
  puVar17[7] = uVar8;
  puVar17[8] = uVar9;
  uVar7 = gte_stTRX();
  uVar8 = gte_stTRY();
  uVar9 = gte_stTRZ();
  puVar17[9] = uVar7;
  puVar17[10] = uVar8;
  puVar17[0xb] = uVar9;
  r0_01 = (VECTOR *)(puVar17 + 0x11);
  *puVar17 = 0;
  puVar17[2] = 0;
  *(short *)(param_4 + 0x96) = (short)*(char *)(param_4 + 0x99) + *(short *)(param_4 + 0x96);
  psVar12 = (short *)(param_4 + 0x96);
  if (iVar11 != 0) {
    iVar13 = (int)*(short *)(param_4 + 0x58);
    puVar17[0x16] = -iVar13;
    psVar3 = psVar16 + -2;
    do {
      gte_SetRotMatrix(r0_02);
      gte_SetTransMatrix(r0_02);
      gte_ldv3(r0_03,pSVar14,pSVar6);
      gte_rtpt();
      iVar11 = iVar11 + -1;
      pSVar6 = (SVECTOR *)(iVar10 + *psVar16 * 8);
      pSVar15 = (SVECTOR *)(iVar10 + psVar3[4] * 8);
      pSVar14 = (SVECTOR *)(iVar10 + psVar3[6] * 8);
      gte_nclip();
      psVar4 = psVar3 + 8;
      psVar16 = psVar16 + 8;
      r0_03 = (SVECTOR *)(iVar5 + psVar3[0xb] * 8);
      gte_stopz((long *)((int)puVar17 + 0x50));
      if (*(int *)((int)puVar17 + 0x50) < 0) {
        pSVar14 = (SVECTOR *)(iVar5 + psVar4[5] * 8);
        pSVar6 = (SVECTOR *)(iVar5 + psVar4[7] * 8);
      }
      else {
        gte_ldv3(pSVar6,pSVar15,pSVar14);
        gte_ncct();
        pSVar14 = (SVECTOR *)(iVar5 + psVar4[5] * 8);
        pSVar6 = (SVECTOR *)(iVar5 + psVar4[7] * 8);
        pSVar15 = (SVECTOR *)(iVar5 + psVar4[1] * 8);
        *(int *)((int)puVar17 + 4) = (int)*psVar12;
        if (0 < iVar13 + pSVar15->vy + (int)*psVar12) {
          *(int *)((int)puVar17 + 4) = *(int *)((int)puVar17 + 0x58) - (int)pSVar15->vy;
        }
        gte_stsxy3_gt3(param_2);
        gte_strgb3_gt3(param_2);
        gte_SetRotMatrix((MATRIX *)&DAT_800b5288);
        gte_ldVXY0(CONCAT22(*(undefined2 *)((int)puVar17 + 4),*(undefined2 *)puVar17));
        gte_ldVZ0((undefined2 *)((int)puVar17 + 8));
        gte_rtv0();
        r0 = param_2 + 0xb;
        r0_00 = (CVECTOR *)(param_2 + 10);
        gte_stlvnl(r0_01);
        r0_01->vx = r0_01->vx + *(int *)((int)puVar17 + 0x24);
        r0_01->vy = r0_01->vy + *(int *)((int)puVar17 + 0x28);
        r0_01->vz = r0_01->vz + *(int *)((int)puVar17 + 0x2c);
        gte_SetRotMatrix(r0_02);
        gte_SetTransMatrix((MATRIX *)((int)puVar17 + 0x30));
        gte_ldv0(pSVar15);
        gte_rtps();
        pSVar15 = (SVECTOR *)(iVar10 + *psVar4 * 8);
        gte_stsxy((long *)r0);
        gte_avsz4();
        gte_stotz((long *)((int)puVar17 + 0x54));
        if (*(uint *)((int)puVar17 + 0x54) >> 6 != 0) {
          gte_ldv0(pSVar15);
          gte_nccs();
          uVar1 = *(uint *)((int)puVar17 + 0x54);
          *(uint *)((int)puVar17 + 0x54) = *(uint *)(puVar18 + (uVar1 >> 4) * 4);
          *(uint *)(puVar18 + (uVar1 >> 4) * 4) = (uint)param_2 & uVar2;
          gte_strgb(r0_00);
          *param_2 = uVar2 & *(uint *)((int)puVar17 + 0x54) | 0xc000000;
        }
      }
      param_2 = param_2 + 0x1a;
      psVar3 = psVar4;
    } while (iVar11 != 0);
  }
  return;
}
