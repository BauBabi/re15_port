/* FUN_80027bec @ 0x80027bec  (Ghidra headless, raw MIPS overlay) */

void FUN_80027bec(int *param_1,u_long *param_2,undefined3 param_3,char param_4)

{
  short sVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  u_long *puVar5;
  int iVar6;
  SVECTOR *pSVar7;
  short *psVar8;
  short *psVar9;
  SVECTOR *pSVar10;
  SVECTOR *r1;
  int iVar11;
  SVECTOR *r0;
  undefined *puVar12;
  uint *r0_00;
  CVECTOR local_res8 [2];
  undefined1 local_8 [8];
  
  r0_00 = (uint *)local_8;
  psVar8 = (short *)param_1[4];
  iVar6 = *param_1;
  local_res8[0] = (CVECTOR)(CONCAT13(param_4 << 1,param_3) | 0x34000000);
  iVar11 = param_1[5];
  puVar12 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  gte_ldrgb(local_res8);
  sVar1 = psVar8[3];
  r0 = (SVECTOR *)(iVar6 + psVar8[1] * 8);
  sVar2 = psVar8[5];
LAB_80027c48:
  pSVar10 = (SVECTOR *)(iVar6 + sVar1 * 8);
  pSVar7 = (SVECTOR *)(iVar6 + sVar2 * 8);
  do {
    psVar9 = psVar8;
    if (iVar11 == 0) {
      return;
    }
    while( true ) {
      gte_ldv3(r0,pSVar10,pSVar7);
      psVar8 = psVar9 + 6;
      iVar11 = iVar11 + -1;
      gte_rtpt_b();
      iVar4 = param_1[2];
      r1 = (SVECTOR *)(iVar4 + psVar9[2] * 8);
      pSVar10 = (SVECTOR *)(iVar4 + psVar9[4] * 8);
      pSVar7 = (SVECTOR *)(iVar4 + *psVar9 * 8);
      gte_nclip_b();
      r0 = (SVECTOR *)(iVar6 + psVar9[7] * 8);
      gte_stopz((long *)r0_00);
      if ((int)*r0_00 < 1) {
        param_2 = param_2 + 0x14;
        sVar1 = psVar8[3];
        sVar2 = psVar8[5];
        goto LAB_80027c48;
      }
      gte_avsz3_b();
      gte_stsxy3_gt3(param_2);
      gte_stotz((long *)r0_00);
      gte_ldv3(pSVar7,r1,pSVar10);
      gte_ncct();
      pSVar10 = (SVECTOR *)(iVar6 + psVar8[3] * 8);
      pSVar7 = (SVECTOR *)(iVar6 + psVar8[5] * 8);
      if ((int)*r0_00 < 8) break;
      if (0x1fff < (int)*r0_00) {
        *r0_00 = 0x1fff;
      }
      uVar3 = *r0_00;
      *r0_00 = *(uint *)(puVar12 + (uVar3 >> 1 & 0xffc));
      *(uint *)(puVar12 + (uVar3 >> 1 & 0xffc)) = (uint)param_2 & 0xffffff;
      *param_2 = *r0_00 & 0xffffff | 0x9000000;
      puVar5 = param_2 + 0x14;
      gte_strgb3_gt3(param_2);
      param_2 = puVar5;
      psVar9 = psVar8;
      if (iVar11 == 0) {
        return;
      }
    }
    param_2 = param_2 + 0x14;
  } while( true );
}


