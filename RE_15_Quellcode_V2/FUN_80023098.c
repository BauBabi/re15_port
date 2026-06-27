void FUN_80023098(int *param_1,uint *param_2,CVECTOR *param_3,char param_4)

{
  short sVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint *r0;
  int iVar5;
  int iVar6;
  SVECTOR *r2;
  SVECTOR *r1;
  undefined *puVar7;
  uint uVar8;
  int iVar9;
  SVECTOR *r0_00;
  int *r0_01;
  undefined1 local_18 [8];
  
  r0_01 = (int *)local_18;
  iVar9 = param_1[5];
  iVar6 = *param_1;
  iVar4 = param_1[4];
  uVar2 = (uint)DAT_800aca34;
  param_3->cd = param_4 << 1 | 0x3c;
  puVar7 = &DAT_800aa6d8 + uVar2 * 0x1000;
  gte_ldrgb(param_3);
  uVar2 = 0xffffff;
  uVar8 = 0xc000000;
  r0_00 = (SVECTOR *)(iVar6 + *(short *)(iVar4 + 2) * 8);
  r1 = (SVECTOR *)(iVar6 + *(short *)(iVar4 + 6) * 8);
  r2 = (SVECTOR *)(iVar6 + *(short *)(iVar4 + 10) * 8);
  if (iVar9 != 0) {
    iVar4 = iVar4 + -4;
    do {
      gte_ldv3(r0_00,r1,r2);
      gte_rtpt();
      iVar9 = iVar9 + -1;
      gte_stsxy3_gt3(param_2);
      gte_nclip();
      iVar5 = iVar4 + 0x10;
      r0_00 = (SVECTOR *)(iVar6 + *(short *)(iVar4 + 0x16) * 8);
      gte_stopz(r0_01);
      r0 = param_2 + 0xb;
      if (*r0_01 < 0) {
        r1 = (SVECTOR *)(iVar6 + *(short *)(iVar5 + 10) * 8);
        r2 = (SVECTOR *)(iVar6 + *(short *)(iVar5 + 0xe) * 8);
      }
      else {
        r1 = (SVECTOR *)(iVar6 + *(short *)(iVar5 + 10) * 8);
        sVar1 = *(short *)(iVar5 + 2);
        r2 = (SVECTOR *)(iVar6 + *(short *)(iVar5 + 0xe) * 8);
        *(CVECTOR *)(param_2 + 1) = *param_3;
        gte_ldv0((SVECTOR *)(iVar6 + sVar1 * 8));
        gte_rtps();
        gte_stsxy((long *)r0);
        gte_avsz4();
        gte_stotz((long *)((int)r0_01 + 4));
        uVar3 = *(uint *)((int)r0_01 + 4);
        if (uVar3 >> 6 != 0) {
          *(uint *)((int)r0_01 + 4) = *(uint *)(puVar7 + (uVar3 >> 4) * 4);
          *(uint *)(puVar7 + (uVar3 >> 4) * 4) = (uint)param_2 & uVar2;
          *param_2 = uVar2 & *(uint *)((int)r0_01 + 4) | uVar8;
        }
      }
      param_2 = param_2 + 0x1a;
      iVar4 = iVar5;
    } while (iVar9 != 0);
  }
  return;
}
