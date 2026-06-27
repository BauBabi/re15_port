void FUN_80022f0c(int *param_1,uint *param_2,CVECTOR *param_3,char param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  short *psVar4;
  short *psVar5;
  SVECTOR *r0;
  int iVar6;
  int iVar7;
  SVECTOR *r2;
  SVECTOR *r1;
  uint uVar8;
  undefined *puVar9;
  int *r0_00;
  undefined1 local_10 [8];
  
  r0_00 = (int *)local_10;
  iVar7 = param_1[5];
  iVar6 = *param_1;
  iVar3 = param_1[4];
  uVar1 = (uint)DAT_800aca34;
  param_3->cd = param_4 << 1 | 0x34;
  puVar9 = &DAT_800aa6d8 + uVar1 * 0x1000;
  gte_ldrgb(param_3);
  uVar8 = 0xffffff;
  uVar1 = 0x9000000;
  r0 = (SVECTOR *)(iVar6 + *(short *)(iVar3 + 2) * 8);
  r1 = (SVECTOR *)(iVar6 + *(short *)(iVar3 + 6) * 8);
  r2 = (SVECTOR *)(iVar6 + *(short *)(iVar3 + 10) * 8);
  if (iVar7 != 0) {
    psVar4 = (short *)(iVar3 + 2);
    do {
      gte_ldv3(r0,r1,r2);
      gte_rtpt();
      psVar5 = psVar4 + 6;
      iVar7 = iVar7 + -1;
      r1 = (SVECTOR *)(iVar6 + psVar4[8] * 8);
      r2 = (SVECTOR *)(iVar6 + psVar4[10] * 8);
      gte_stsxy3_gt3(param_2);
      gte_nclip();
      r0 = (SVECTOR *)(iVar6 + *psVar5 * 8);
      gte_stopz(r0_00);
      if (-1 < *r0_00) {
        gte_avsz3();
        *(CVECTOR *)(param_2 + 1) = *param_3;
        gte_stotz(r0_00 + 1);
        uVar2 = *(uint *)((int)r0_00 + 4);
        if (uVar2 >> 6 != 0) {
          *(uint *)((int)r0_00 + 4) = *(uint *)(puVar9 + (uVar2 >> 4) * 4);
          *(uint *)(puVar9 + (uVar2 >> 4) * 4) = (uint)param_2 & uVar8;
          *param_2 = uVar8 & *(uint *)((int)r0_00 + 4) | uVar1;
        }
      }
      param_2 = param_2 + 0x14;
      psVar4 = psVar5;
    } while (iVar7 != 0);
  }
  return;
}
