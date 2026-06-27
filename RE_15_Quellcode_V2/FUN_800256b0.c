void FUN_800256b0(int *param_1,uint *param_2,CVECTOR *param_3,char param_4)

{
  uint uVar1;
  int iVar2;
  SVECTOR *pSVar3;
  u_long *r0;
  short *psVar4;
  short *psVar5;
  int iVar6;
  SVECTOR *pSVar7;
  SVECTOR *pSVar8;
  short *psVar9;
  int iVar10;
  SVECTOR *r0_00;
  undefined *puVar11;
  uint uVar12;
  CVECTOR *r0_01;
  int iVar13;
  uint *r0_02;
  undefined1 local_18 [8];
  
  r0_02 = (uint *)local_18;
  psVar9 = (short *)param_1[4];
  iVar13 = param_1[5];
  iVar6 = *param_1;
  iVar10 = param_1[2];
  uVar1 = (uint)DAT_800aca34;
  param_3->cd = param_4 << 1 | 0x3c;
  puVar11 = &DAT_800aa6d8 + uVar1 * 0x1000;
  gte_ldrgb(param_3);
  uVar1 = 0xffffff;
  uVar12 = 0xc000000;
  r0_00 = (SVECTOR *)(iVar6 + psVar9[1] * 8);
  pSVar7 = (SVECTOR *)(iVar6 + psVar9[3] * 8);
  pSVar3 = (SVECTOR *)(iVar6 + psVar9[5] * 8);
  if (iVar13 != 0) {
    psVar4 = psVar9 + -2;
    do {
      gte_ldv3(r0_00,pSVar7,pSVar3);
      gte_rtpt();
      iVar13 = iVar13 + -1;
      pSVar7 = (SVECTOR *)(iVar10 + *psVar9 * 8);
      pSVar8 = (SVECTOR *)(iVar10 + psVar4[4] * 8);
      pSVar3 = (SVECTOR *)(iVar10 + psVar4[6] * 8);
      gte_nclip();
      psVar5 = psVar4 + 8;
      psVar9 = psVar9 + 8;
      r0_00 = (SVECTOR *)(iVar6 + psVar4[0xb] * 8);
      gte_stopz((long *)r0_02);
      if ((int)*r0_02 < 0) {
        pSVar7 = (SVECTOR *)(iVar6 + psVar5[5] * 8);
        pSVar3 = (SVECTOR *)(iVar6 + psVar5[7] * 8);
      }
      else {
        gte_ldv3(pSVar7,pSVar8,pSVar3);
        gte_ncct();
        r0 = param_2 + 0xb;
        r0_01 = (CVECTOR *)(param_2 + 10);
        pSVar7 = (SVECTOR *)(iVar6 + psVar5[5] * 8);
        pSVar3 = (SVECTOR *)(iVar6 + psVar5[7] * 8);
        pSVar8 = (SVECTOR *)(iVar6 + psVar5[1] * 8);
        gte_stsxy3_gt3(param_2);
        gte_strgb3_gt3(param_2);
        gte_ldv0(pSVar8);
        gte_rtps();
        pSVar8 = (SVECTOR *)(iVar10 + *psVar5 * 8);
        gte_stsxy((long *)r0);
        gte_avsz4();
        gte_stotz((long *)r0_02);
        if ((int)*r0_02 >> 6 != 0) {
          gte_ldv0(pSVar8);
          gte_nccs();
          iVar2 = *r0_02;
          *r0_02 = *(uint *)(puVar11 + (iVar2 >> 4) * 4);
          *(uint *)(puVar11 + (iVar2 >> 4) * 4) = (uint)param_2 & uVar1;
          gte_strgb(r0_01);
          *param_2 = uVar1 & *r0_02 | uVar12;
        }
      }
      param_2 = param_2 + 0x1a;
      psVar4 = psVar5;
    } while (iVar13 != 0);
  }
  return;
}
