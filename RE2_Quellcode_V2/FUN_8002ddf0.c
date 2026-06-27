/* FUN_8002ddf0 @ 0x8002ddf0  (Ghidra headless, raw MIPS overlay) */

void FUN_8002ddf0(int *param_1,u_long *param_2,int param_3)

{
  short sVar1;
  uint uVar2;
  MATRIX *r0;
  int iVar3;
  u_long *r0_00;
  int iVar4;
  int iVar5;
  SVECTOR *pSVar6;
  SVECTOR *pSVar7;
  SVECTOR *r1;
  SVECTOR *r0_01;
  int iVar8;
  int iVar9;
  int iVar10;
  undefined *puVar11;
  VECTOR *r0_02;
  int iVar12;
  VECTOR *r0_03;
  undefined4 local_78;
  int local_74;
  undefined4 local_70;
  MATRIX aMStack_68 [2];
  
  r0_03 = (VECTOR *)&local_78;
  iVar10 = (int)*(short *)(DAT_800ce330 + 0x1c2);
  local_78 = 0;
  iVar12 = (int)*(short *)(param_3 + 0x9a);
  local_70 = 0;
  local_74 = iVar12;
  gte_ReadRotMatrix(aMStack_68);
  r0_02 = (VECTOR *)((int)r0_03 + 0x44);
  iVar4 = param_1[4];
  iVar8 = param_1[5];
  sVar1 = *(short *)(iVar4 + 2);
  *(uint *)((int)r0_03 + 0x50) = *(uint *)(param_3 + 0x70) & 0xffffff | 0x3c000000;
  iVar5 = *param_1;
  iVar9 = *(int *)(param_3 + 0x60);
  r0_01 = (SVECTOR *)(iVar5 + sVar1 * 8);
  pSVar7 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 6) * 8);
  pSVar6 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 10) * 8);
  puVar11 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  gte_ldrgb((CVECTOR *)((int)r0_03 + 0x50));
  do {
    r0_00 = param_2;
    iVar3 = iVar4;
    if (iVar8 == 0) {
      return;
    }
    while( true ) {
      gte_ldv3(r0_01,pSVar7,pSVar6);
      iVar4 = iVar3 + 0x10;
      iVar8 = iVar8 + -1;
      gte_rtpt_b();
      pSVar6 = (SVECTOR *)(iVar5 + *(short *)(iVar3 + 0xe) * 8);
      gte_nclip_b();
      gte_stopz((long *)((int)r0_03 + 0x54));
      gte_stsxy3_gt3(r0_00);
      gte_ldv0(pSVar6);
      gte_rtps();
      iVar3 = param_1[2];
      gte_ldv0((SVECTOR *)(iVar3 + *(short *)(iVar4 + -4) * 8));
      gte_nccs();
      pSVar6 = (SVECTOR *)(iVar3 + *(short *)(iVar4 + -0x10) * 8);
      r1 = (SVECTOR *)(iVar3 + *(short *)(iVar4 + -0xc) * 8);
      pSVar7 = (SVECTOR *)(iVar3 + *(short *)(iVar4 + -8) * 8);
      gte_avsz4_b();
      gte_stsxy((long *)(r0_00 + 0xb));
      gte_strgb((CVECTOR *)(r0_00 + 10));
      gte_stotz((long *)((int)r0_03 + 0x58));
      gte_ldv3(pSVar6,r1,pSVar7);
      gte_ncct();
      pSVar7 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 6) * 8);
      pSVar6 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 10) * 8);
      if (0x1fff < *(int *)((int)r0_03 + 0x58)) {
        *(undefined4 *)((int)r0_03 + 0x58) = 0x1fff;
      }
      uVar2 = *(uint *)((int)r0_03 + 0x58);
      *(uint *)((int)r0_03 + 0x58) = *(uint *)(puVar11 + (uVar2 >> 1 & 0xffc));
      *(uint *)(puVar11 + (uVar2 >> 1 & 0xffc)) = (uint)r0_00 & 0xffffff;
      *r0_00 = *(uint *)((int)r0_03 + 0x58) & 0xffffff | 0xc000000;
      param_2 = r0_00 + 0x1a;
      gte_strgb3_gt3(r0_00);
      if (*(int *)((int)r0_03 + 0x54) < 0) break;
      *(int *)((int)r0_03 + 4) = iVar12;
      if (iVar10 < iVar9 + r0_01->vy + iVar12) {
        *(int *)((int)r0_03 + 4) = (-(int)r0_01->vy - iVar9) + iVar10;
      }
      gte_SetRotMatrix((MATRIX *)&DAT_800dcba8);
      gte_ldlv0(r0_03);
      gte_rt();
      gte_stlvnl(r0_02);
      gte_SetTransMatrix((MATRIX *)((int)r0_03 + 0x30));
      gte_ldv0(r0_01);
      gte_rtps();
      r0_01 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 2) * 8);
      gte_stsxy((long *)(param_2 + -0x18));
      r0 = (MATRIX *)((int)r0_03 + 0x10);
      gte_SetRotMatrix(r0);
      gte_SetTransMatrix(r0);
      r0_00 = param_2;
      iVar3 = iVar4;
      if (iVar8 == 0) {
        return;
      }
    }
    r0_01 = (SVECTOR *)(iVar5 + *(short *)(iVar4 + 2) * 8);
  } while( true );
}


