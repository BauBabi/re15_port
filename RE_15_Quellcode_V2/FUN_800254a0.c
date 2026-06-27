void FUN_800254a0(int *param_1,uint *param_2,CVECTOR *param_3,char param_4)

{
  uint uVar1;
  int iVar2;
  short *psVar3;
  short *psVar4;
  SVECTOR *pSVar5;
  SVECTOR *pSVar6;
  SVECTOR *r1;
  int iVar7;
  short *psVar8;
  int iVar9;
  SVECTOR *r0;
  undefined *puVar10;
  int iVar11;
  uint uVar12;
  uint *r0_00;
  undefined1 local_10 [8];
  
  r0_00 = (uint *)local_10;
  psVar8 = (short *)param_1[4];
  iVar9 = param_1[5];
  iVar7 = *param_1;
  iVar11 = param_1[2];
  uVar1 = (uint)DAT_800aca34;
  param_3->cd = param_4 << 1 | 0x34;
  puVar10 = &DAT_800aa6d8 + uVar1 * 0x1000;
  gte_ldrgb(param_3);
  uVar12 = 0xffffff;
  uVar1 = 0x9000000;
  r0 = (SVECTOR *)(iVar7 + psVar8[1] * 8);
  pSVar6 = (SVECTOR *)(iVar7 + psVar8[3] * 8);
  pSVar5 = (SVECTOR *)(iVar7 + psVar8[5] * 8);
  if (iVar9 != 0) {
    psVar3 = psVar8 + 5;
    do {
      gte_ldv3(r0,pSVar6,pSVar5);
      gte_rtpt();
      iVar9 = iVar9 + -1;
      r1 = (SVECTOR *)(iVar11 + psVar3[-3] * 8);
      pSVar6 = (SVECTOR *)(iVar11 + psVar3[-1] * 8);
      pSVar5 = (SVECTOR *)(iVar11 + *psVar8 * 8);
      gte_nclip();
      psVar4 = psVar3 + 6;
      psVar8 = psVar8 + 6;
      r0 = (SVECTOR *)(iVar7 + psVar3[2] * 8);
      gte_stopz((long *)r0_00);
      if ((int)*r0_00 < 0) {
        pSVar6 = (SVECTOR *)(iVar7 + psVar4[-2] * 8);
        pSVar5 = (SVECTOR *)(iVar7 + *psVar4 * 8);
      }
      else {
        gte_ldv3(pSVar5,r1,pSVar6);
        gte_ncct();
        pSVar6 = (SVECTOR *)(iVar7 + psVar4[-2] * 8);
        pSVar5 = (SVECTOR *)(iVar7 + *psVar4 * 8);
        gte_stsxy3_gt3(param_2);
        gte_strgb3_gt3(param_2);
        gte_avsz3();
        gte_stotz((long *)r0_00);
        iVar2 = *r0_00;
        if (iVar2 >> 6 != 0) {
          *r0_00 = *(uint *)(puVar10 + (iVar2 >> 4) * 4);
          *(uint *)(puVar10 + (iVar2 >> 4) * 4) = (uint)param_2 & uVar12;
          *param_2 = uVar12 & *r0_00 | uVar1;
        }
      }
      param_2 = param_2 + 0x14;
      psVar3 = psVar4;
    } while (iVar9 != 0);
  }
  return;
}
