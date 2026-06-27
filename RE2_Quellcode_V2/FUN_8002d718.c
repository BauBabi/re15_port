/* FUN_8002d718 @ 0x8002d718  (Ghidra headless, raw MIPS overlay) */

void FUN_8002d718(int *param_1,uint *param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  SVECTOR *r2;
  uint uVar4;
  short *psVar5;
  uint *r0;
  ushort *puVar6;
  ushort *puVar7;
  SVECTOR *r0_00;
  VECTOR *r0_01;
  ushort *puVar8;
  ushort *puVar9;
  int iVar10;
  SVECTOR *r0_02;
  int iVar11;
  undefined *puVar12;
  int iVar13;
  SVECTOR *r1;
  MATRIX *r0_03;
  MATRIX aMStack_60 [2];
  
  r0_03 = aMStack_60;
  puVar8 = (ushort *)param_1[4];
  iVar13 = param_1[5];
  iVar10 = *param_1;
  iVar11 = param_1[2];
  puVar12 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  uVar2 = *(ushort *)(param_3 + 0xa0) & 0x7fff;
  uVar4 = (uint)(*(ushort *)(param_3 + 0xa0) >> 0xf);
  gte_ReadRotMatrix(aMStack_60);
  puVar7 = (ushort *)(iVar11 + (uint)*puVar8 * 8);
  psVar5 = (short *)(&DAT_8009dc28 + uVar2 * 2 + uVar4 * 0x3c);
  iVar3 = (int)*psVar5;
  *(int *)((int)r0_03 + 0x34) = ((int)((uint)*puVar7 << 0x10) >> 0x1a) * iVar3;
  *(int *)((int)r0_03 + 0x38) = ((int)((uint)puVar7[1] << 0x10) >> 0x1a) * iVar3;
  r0_01 = (VECTOR *)((int)r0_03 + 0x34);
  *(int *)((int)r0_03 + 0x3c) = ((int)((uint)puVar7[2] << 0x10) >> 0x1a) * iVar3;
  gte_ldlv0(r0_01);
  uVar2 = *(uint *)(param_3 + 0x70) & 0xffffff | 0x3c000000;
  copFunction(2,0x480012);
  r0_02 = (SVECTOR *)(iVar10 + (uint)puVar8[1] * 8);
  r1 = (SVECTOR *)(iVar10 + (uint)puVar8[3] * 8);
  r2 = (SVECTOR *)(iVar10 + (uint)puVar8[5] * 8);
  if (iVar13 != 0) {
    uVar4 = 0xffffff;
    r0 = param_2 + 0xb;
    puVar7 = puVar8 + -2;
    do {
      gte_stlvnl(r0_01);
      gte_SetTransMatrix((MATRIX *)((int)r0_03 + 0x20));
      gte_ldv3(r0_02,r1,r2);
      iVar13 = iVar13 + -1;
      puVar6 = puVar7 + 8;
      puVar9 = puVar8 + 8;
      gte_rtpt_b();
      r0_02 = (SVECTOR *)(iVar10 + (uint)puVar8[9] * 8);
      r1 = (SVECTOR *)(iVar10 + (uint)puVar7[0xd] * 8);
      r2 = (SVECTOR *)(iVar10 + (uint)puVar7[0xf] * 8);
      r0_00 = (SVECTOR *)(iVar10 + (uint)puVar7[9] * 8);
      gte_stsxy3_gt3(param_2);
      gte_ldv0(r0_00);
      gte_rtps();
      r0[-10] = uVar2;
      r0[-7] = uVar2;
      r0[-4] = uVar2;
      r0[-1] = uVar2;
      gte_avsz4_b();
      gte_stsxy((long *)r0);
      gte_stotz((long *)((int)r0_03 + 0x40));
      puVar8 = (ushort *)(iVar11 + (uint)*puVar9 * 8);
      gte_SetTransMatrix(r0_03);
      iVar3 = (int)*psVar5;
      r0_01->vx = ((int)((uint)*puVar8 << 0x10) >> 0x1a) * iVar3;
      r0_01->vy = ((int)((uint)puVar8[1] << 0x10) >> 0x1a) * iVar3;
      r0_01->vz = ((int)((uint)puVar8[2] << 0x10) >> 0x1a) * iVar3;
      gte_ldlv0(r0_01);
      gte_rt();
      if (0x1fff < *(int *)((int)r0_03 + 0x40)) {
        *(undefined4 *)((int)r0_03 + 0x40) = 0x1fff;
      }
      uVar1 = *(uint *)((int)r0_03 + 0x40);
      r0 = r0 + 0x1a;
      *(uint *)((int)r0_03 + 0x40) = *(uint *)(puVar12 + (uVar1 >> 1 & 0xffc));
      *(uint *)(puVar12 + (uVar1 >> 1 & 0xffc)) = (uint)param_2 & uVar4;
      *param_2 = *(uint *)((int)r0_03 + 0x40) & uVar4 | 0xc000000;
      param_2 = param_2 + 0x1a;
      puVar7 = puVar6;
      puVar8 = puVar9;
    } while (iVar13 != 0);
  }
  return;
}


