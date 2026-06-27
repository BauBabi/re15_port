/* FUN_8002d3c8 @ 0x8002d3c8  (Ghidra headless, raw MIPS overlay) */

void FUN_8002d3c8(int *param_1,uint *param_2,int param_3)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  short *psVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  uint uVar9;
  SVECTOR *r2;
  ushort *puVar10;
  VECTOR *r0;
  uint *puVar11;
  SVECTOR *r1;
  SVECTOR *r0_00;
  int iVar12;
  undefined *puVar13;
  int iVar14;
  int iVar15;
  MATRIX *r0_01;
  undefined1 auStack_68 [32];
  MATRIX MStack_48;
  
  r0_01 = (MATRIX *)auStack_68;
  puVar11 = (uint *)param_1[4];
  iVar15 = param_1[5];
  iVar12 = param_1[2];
  iVar14 = *param_1;
  uVar9 = *(ushort *)(param_3 + 0xa0) & 0x7fff;
  uVar4 = (uint)(*(ushort *)(param_3 + 0xa0) >> 0xf);
  puVar10 = (ushort *)(iVar12 + (uint)(ushort)*puVar11 * 8);
  puVar13 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  gte_ReadRotMatrix(&MStack_48);
  psVar5 = (short *)(&DAT_8009dc28 + uVar9 * 2 + uVar4 * 0x3c);
  iVar3 = (int)*psVar5;
  *(int *)((int)r0_01 + 0x14) = ((int)((uint)*puVar10 << 0x10) >> 0x1a) * iVar3;
  *(int *)((int)r0_01 + 0x18) = ((int)((uint)puVar10[1] << 0x10) >> 0x1a) * iVar3;
  uVar1 = puVar10[2];
  r0 = (VECTOR *)((int)r0_01 + 0x14);
  *(int *)((int)r0_01 + 0x40) = iVar3;
  *(int *)((int)r0_01 + 0x1c) = ((int)((uint)uVar1 << 0x10) >> 0x1a) * iVar3;
  gte_ldlv0(r0);
  uVar4 = *(uint *)(param_3 + 0x70) & 0xffffff | 0x34000000;
  copFunction(2,0x480012);
  r0_00 = (SVECTOR *)(iVar14 + (uint)*(ushort *)((int)puVar11 + 2) * 8);
  r1 = (SVECTOR *)(iVar14 + (uint)*(ushort *)((int)puVar11 + 6) * 8);
  r2 = (SVECTOR *)(iVar14 + (uint)*(ushort *)((int)puVar11 + 10) * 8);
  if (iVar15 != 0) {
    uVar9 = 0xffffff;
    puVar6 = param_2 + 7;
    puVar7 = puVar11 + 2;
    do {
      gte_stlvnl(r0);
      gte_SetTransMatrix(r0_01);
      gte_ldv3(r0_00,r1,r2);
      puVar8 = puVar7 + 3;
      puVar11 = puVar11 + 3;
      iVar15 = iVar15 + -1;
      gte_rtpt_b();
      r0_00 = (SVECTOR *)(iVar14 + (*puVar11 >> 0x10) * 8);
      r1 = (SVECTOR *)(iVar14 + (uint)*(ushort *)((int)puVar7 + 10) * 8);
      puVar10 = (ushort *)(iVar12 + (*puVar11 & 0xffff) * 8);
      r2 = (SVECTOR *)(iVar14 + (uint)*(ushort *)((int)puVar7 + 0xe) * 8);
      gte_avsz3_b();
      puVar6[-6] = uVar4;
      puVar6[-3] = uVar4;
      *puVar6 = uVar4;
      gte_stsxy3_gt3(param_2);
      gte_stotz((long *)((int)r0_01 + 0x40));
      gte_SetTransMatrix((MATRIX *)((int)r0_01 + 0x20));
      iVar3 = (int)*psVar5;
      r0->vx = ((int)((uint)*puVar10 << 0x10) >> 0x1a) * iVar3;
      r0->vy = ((int)((uint)puVar10[1] << 0x10) >> 0x1a) * iVar3;
      r0->vz = ((int)((uint)puVar10[2] << 0x10) >> 0x1a) * iVar3;
      gte_ldlv0(r0);
      gte_rt();
      if (0x1fff < *(int *)((int)r0_01 + 0x40)) {
        *(undefined4 *)((int)r0_01 + 0x40) = 0x1fff;
      }
      uVar2 = *(uint *)((int)r0_01 + 0x40);
      puVar6 = puVar6 + 0x14;
      *(uint *)((int)r0_01 + 0x40) = *(uint *)(puVar13 + (uVar2 >> 1 & 0xffc));
      *(uint *)(puVar13 + (uVar2 >> 1 & 0xffc)) = (uint)param_2 & uVar9;
      *param_2 = *(uint *)((int)r0_01 + 0x40) & uVar9 | 0x9000000;
      param_2 = param_2 + 0x14;
      puVar7 = puVar8;
    } while (iVar15 != 0);
  }
  return;
}


