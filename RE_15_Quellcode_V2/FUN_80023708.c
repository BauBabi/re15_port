void FUN_80023708(int *param_1,uint *param_2,int param_3)

{
  SVECTOR *r0;
  int iVar1;
  u_long *r0_00;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  VECTOR *r0_01;
  SVECTOR *r1;
  uint uVar6;
  short *psVar7;
  uint uVar8;
  SVECTOR *r2;
  SVECTOR *r0_02;
  uint uVar9;
  undefined *puVar10;
  uint uVar11;
  int iVar12;
  uint *puVar13;
  MATRIX *r0_03;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  VECTOR local_44 [2];
  
  r0_03 = (MATRIX *)&local_78;
  uVar9 = 0xffffff;
  puVar13 = (uint *)param_1[4];
  iVar2 = param_1[5];
  iVar12 = *param_1;
  uVar11 = 0xc000000;
  uVar6 = *(uint *)(param_3 + 0x68) & 0xffffff | 0x3c000000;
  r0_02 = (SVECTOR *)(iVar12 + (uint)*(ushort *)((int)puVar13 + 2) * 8);
  r1 = (SVECTOR *)(iVar12 + (uint)*(ushort *)((int)puVar13 + 6) * 8);
  r2 = (SVECTOR *)(iVar12 + (uint)*(ushort *)((int)puVar13 + 10) * 8);
  puVar10 = &DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000;
  local_78 = gte_stR11R12();
  local_74 = gte_stR13R21();
  local_70 = gte_stR22R23();
  local_6c = gte_stR31R32();
  local_68 = gte_stR33();
  local_64 = gte_stTRX();
  local_60 = gte_stTRY();
  local_5c = gte_stTRZ();
  r0_01 = local_44;
  if (iVar2 != 0) {
    psVar7 = (short *)(&DAT_80073454 + ((int)((uint)*(ushort *)(param_3 + 0x9c) << 0x10) >> 0xf));
    uVar8 = 0xffff0000;
    puVar3 = param_2 + 10;
    puVar4 = puVar13 + -1;
    r0_03 = (MATRIX *)&local_78;
    do {
      gte_SetTransMatrix(r0_03);
      r0_01->vx = ((int)((uint)(ushort)r1->vx << 0x10) >> 0x15) * (int)*psVar7;
      r0_01->vy = (r1->vy + 300 >> 6) * (int)*psVar7;
      r0_01->vz = ((int)((uint)(ushort)r1->vz << 0x10) >> 0x15) * (int)*psVar7;
      gte_ldVXY0(CONCAT22((short)r0_01->vy,(short)r0_01->vx));
      gte_ldVZ0(&r0_01->vz);
      gte_rt();
      iVar2 = iVar2 + -1;
      gte_stlvnl(r0_01);
      gte_SetTransMatrix((MATRIX *)((int)r0_03 + 0x20));
      gte_ldv3(r0_02,r1,r2);
      gte_rtpt();
      puVar5 = puVar4 + 4;
      puVar13 = puVar13 + 4;
      r0_00 = param_2 + 0xb;
      r0_02 = (SVECTOR *)(iVar12 + ((*puVar13 & uVar8) >> 0xd));
      r1 = (SVECTOR *)(iVar12 + ((puVar4[6] & uVar8) >> 0xd));
      r2 = (SVECTOR *)(iVar12 + ((puVar4[7] & uVar8) >> 0xd));
      r0 = (SVECTOR *)(iVar12 + ((*puVar5 & uVar8) >> 0xd));
      gte_stsxy3_gt3(param_2);
      puVar3[-9] = uVar6;
      puVar3[-6] = uVar6;
      puVar3[-3] = uVar6;
      *puVar3 = uVar6;
      gte_ldv0(r0);
      gte_rtps();
      gte_stsxy((long *)r0_00);
      gte_avsz4();
      gte_stotz((long *)((int)r0_03 + 0x40));
      iVar1 = *(int *)((int)r0_03 + 0x40);
      if (iVar1 >> 6 != 0) {
        *(uint *)((int)r0_03 + 0x40) = *(uint *)(puVar10 + (iVar1 >> 4) * 4);
        *(uint *)(puVar10 + (iVar1 >> 4) * 4) = (uint)param_2 & uVar9;
        *param_2 = uVar9 & *(uint *)((int)r0_03 + 0x40) | uVar11;
      }
      puVar3 = puVar3 + 0x1a;
      param_2 = param_2 + 0x1a;
      puVar4 = puVar5;
    } while (iVar2 != 0);
  }
  *(short *)(param_3 + 0x9c) = *(short *)(param_3 + 0x9c) + 1;
  gte_SetTransMatrix(r0_03);
  return;
}
