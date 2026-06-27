/* FUN_8002da80 @ 0x8002da80  (Ghidra headless, raw MIPS overlay) */

void FUN_8002da80(int *param_1,u_long *param_2,int param_3)

{
  uint uVar1;
  MATRIX *r0;
  int iVar2;
  u_long *r0_00;
  int iVar3;
  short *psVar4;
  short *psVar5;
  SVECTOR *pSVar6;
  SVECTOR *r0_01;
  SVECTOR *pSVar7;
  SVECTOR *r1;
  int iVar8;
  int iVar9;
  undefined *puVar10;
  VECTOR *r0_02;
  int iVar11;
  int iVar12;
  VECTOR *r0_03;
  undefined4 local_70;
  int local_6c;
  undefined4 local_68;
  MATRIX aMStack_60 [2];
  
  r0_03 = (VECTOR *)&local_70;
  iVar9 = (int)*(short *)(DAT_800ce330 + 0x1c2);
  local_70 = 0;
  iVar11 = (int)*(short *)(param_3 + 0x98);
  local_68 = 0;
  local_6c = iVar11;
  gte_ReadRotMatrix(aMStack_60);
  r0_02 = (VECTOR *)((int)r0_03 + 0x44);
  psVar4 = (short *)param_1[4];
  iVar8 = param_1[5];
  *(uint *)((int)r0_03 + 0x50) = *(uint *)(param_3 + 0x70) & 0xffffff | 0x34000000;
  iVar12 = *(int *)(param_3 + 0x60);
  iVar3 = *param_1;
  puVar10 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  gte_ldrgb((CVECTOR *)((int)r0_03 + 0x50));
  r0_01 = (SVECTOR *)(iVar3 + psVar4[1] * 8);
  pSVar7 = (SVECTOR *)(iVar3 + psVar4[3] * 8);
  pSVar6 = (SVECTOR *)(iVar3 + psVar4[5] * 8);
  do {
    r0_00 = param_2;
    psVar5 = psVar4;
    if (iVar8 == 0) {
      return;
    }
    while( true ) {
      gte_ldv3(r0_01,pSVar7,pSVar6);
      psVar4 = psVar5 + 6;
      iVar8 = iVar8 + -1;
      gte_rtpt_b();
      iVar2 = param_1[2];
      r1 = (SVECTOR *)(iVar2 + psVar5[2] * 8);
      pSVar7 = (SVECTOR *)(iVar2 + psVar5[4] * 8);
      pSVar6 = (SVECTOR *)(iVar2 + *psVar5 * 8);
      gte_nclip_b();
      gte_stopz((long *)((int)r0_03 + 0x54));
      gte_avsz3();
      gte_stsxy3_gt3(r0_00);
      gte_stotz((long *)((int)r0_03 + 0x58));
      gte_ldv3(pSVar6,r1,pSVar7);
      gte_ncct();
      pSVar7 = (SVECTOR *)(iVar3 + psVar4[3] * 8);
      pSVar6 = (SVECTOR *)(iVar3 + psVar4[5] * 8);
      if (0x1fff < *(int *)((int)r0_03 + 0x58)) {
        *(undefined4 *)((int)r0_03 + 0x58) = 0x1fff;
      }
      uVar1 = *(uint *)((int)r0_03 + 0x58);
      *(uint *)((int)r0_03 + 0x58) = *(uint *)(puVar10 + (uVar1 >> 1 & 0xffc));
      *(uint *)(puVar10 + (uVar1 >> 1 & 0xffc)) = (uint)r0_00 & 0xffffff;
      *r0_00 = *(uint *)((int)r0_03 + 0x58) & 0xffffff | 0x9000000;
      param_2 = r0_00 + 0x14;
      gte_strgb3_gt3(r0_00);
      if (*(int *)((int)r0_03 + 0x54) < 0) break;
      *(int *)((int)r0_03 + 4) = iVar11;
      if (iVar9 < iVar12 + r0_01->vy + iVar11) {
        *(int *)((int)r0_03 + 4) = (-(int)r0_01->vy - iVar12) + iVar9;
      }
      gte_SetRotMatrix((MATRIX *)&DAT_800dcba8);
      gte_ldlv0(r0_03);
      gte_rt();
      gte_stlvnl(r0_02);
      gte_SetTransMatrix((MATRIX *)((int)r0_03 + 0x30));
      gte_ldv0(r0_01);
      gte_rtps();
      r0_01 = (SVECTOR *)(iVar3 + psVar4[1] * 8);
      gte_stsxy((long *)(param_2 + -0x12));
      r0 = (MATRIX *)((int)r0_03 + 0x10);
      gte_SetRotMatrix(r0);
      gte_SetTransMatrix(r0);
      r0_00 = param_2;
      psVar5 = psVar4;
      if (iVar8 == 0) {
        return;
      }
    }
    r0_01 = (SVECTOR *)(iVar3 + psVar4[1] * 8);
  } while( true );
}


