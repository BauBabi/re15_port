/* FUN_80027dbc @ 0x80027dbc  (Ghidra headless, raw MIPS overlay) */

void FUN_80027dbc(int *param_1,u_long *param_2,undefined3 param_3,char param_4)

{
  uint uVar1;
  undefined *puVar2;
  SVECTOR *pSVar3;
  int iVar4;
  int iVar5;
  u_long *puVar6;
  int iVar7;
  SVECTOR *pSVar8;
  SVECTOR *r1;
  int iVar9;
  SVECTOR *r0;
  uint *r0_00;
  CVECTOR local_res8 [2];
  undefined1 local_8 [8];
  
  r0_00 = (uint *)local_8;
  iVar4 = param_1[4];
  iVar7 = *param_1;
  local_res8[0] = (CVECTOR)(CONCAT13(param_4 << 1,param_3) | 0x3c000000);
  iVar9 = param_1[5];
  puVar2 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  r0 = (SVECTOR *)(iVar7 + *(short *)(iVar4 + 2) * 8);
  pSVar8 = (SVECTOR *)(iVar7 + *(short *)(iVar4 + 6) * 8);
  pSVar3 = (SVECTOR *)(iVar7 + *(short *)(iVar4 + 10) * 8);
  gte_ldrgb(local_res8);
  while (iVar9 != 0) {
    gte_ldv3(r0,pSVar8,pSVar3);
    iVar5 = iVar4 + 0x10;
    iVar9 = iVar9 + -1;
    gte_rtpt_b();
    r0 = (SVECTOR *)(iVar7 + *(short *)(iVar4 + 0x12) * 8);
    gte_nclip_b();
    pSVar3 = (SVECTOR *)(iVar7 + *(short *)(iVar4 + 0xe) * 8);
    gte_stopz((long *)r0_00);
    if ((int)*r0_00 < 0) {
      param_2 = param_2 + 0x1a;
      pSVar8 = (SVECTOR *)(iVar7 + *(short *)(iVar5 + 6) * 8);
      pSVar3 = (SVECTOR *)(iVar7 + *(short *)(iVar5 + 10) * 8);
      iVar4 = iVar5;
    }
    else {
      gte_stsxy3_gt3(param_2);
      gte_ldv0(pSVar3);
      gte_rtps();
      iVar4 = param_1[2];
      gte_ldv0((SVECTOR *)(iVar4 + *(short *)(iVar5 + -4) * 8));
      gte_nccs();
      pSVar3 = (SVECTOR *)(iVar4 + *(short *)(iVar5 + -0x10) * 8);
      r1 = (SVECTOR *)(iVar4 + *(short *)(iVar5 + -0xc) * 8);
      pSVar8 = (SVECTOR *)(iVar4 + *(short *)(iVar5 + -8) * 8);
      gte_avsz4_b();
      gte_stsxy((long *)(param_2 + 0xb));
      gte_strgb((CVECTOR *)(param_2 + 10));
      gte_stotz((long *)r0_00);
      gte_ldv3(pSVar3,r1,pSVar8);
      gte_ncct();
      pSVar8 = (SVECTOR *)(iVar7 + *(short *)(iVar5 + 6) * 8);
      pSVar3 = (SVECTOR *)(iVar7 + *(short *)(iVar5 + 10) * 8);
      if ((int)*r0_00 < 8) {
        param_2 = param_2 + 0x1a;
        iVar4 = iVar5;
      }
      else {
        if (0x1fff < (int)*r0_00) {
          *r0_00 = 0x1fff;
        }
        uVar1 = *r0_00;
        *r0_00 = *(uint *)(puVar2 + (uVar1 >> 1 & 0xffc));
        *(uint *)(puVar2 + (uVar1 >> 1 & 0xffc)) = (uint)param_2 & 0xffffff;
        *param_2 = *r0_00 & 0xffffff | 0xc000000;
        puVar6 = param_2 + 0x1a;
        gte_strgb3_gt3(param_2);
        iVar4 = iVar5;
        param_2 = puVar6;
      }
    }
  }
  return;
}


