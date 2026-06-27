void FUN_80023410(int *param_1,uint *param_2,int param_3)

{
  MATRIX *r0;
  int iVar1;
  uint *puVar2;
  VECTOR *r0_00;
  SVECTOR *r1;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  int iVar7;
  SVECTOR *r2;
  SVECTOR *r0_01;
  uint uVar8;
  uint uVar9;
  undefined *puVar10;
  uint uVar11;
  int iVar12;
  short *psVar13;
  MATRIX *r0_02;
  undefined1 auStack_70 [20];
  undefined1 local_5c [16];
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  
  r0_02 = (MATRIX *)auStack_70;
  uVar9 = 0xffffff;
  puVar6 = (uint *)param_1[4];
  iVar12 = param_1[5];
  iVar7 = *param_1;
  uVar11 = 0x9000000;
  uVar5 = *(uint *)(param_3 + 0x68) & 0xffffff | 0x34000000;
  r0_01 = (SVECTOR *)(iVar7 + (uint)*(ushort *)((int)puVar6 + 2) * 8);
  r1 = (SVECTOR *)(iVar7 + (uint)*(ushort *)((int)puVar6 + 6) * 8);
  r2 = (SVECTOR *)(iVar7 + (uint)*(ushort *)((int)puVar6 + 10) * 8);
  puVar10 = &DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000;
  r0 = (MATRIX *)(local_5c + 0xc);
  local_5c._12_4_ = gte_stR11R12();
  local_4c = gte_stR13R21();
  local_48 = gte_stR22R23();
  local_44 = gte_stR31R32();
  local_40 = gte_stR33();
  local_3c = gte_stTRX();
  local_38 = gte_stTRY();
  local_34 = gte_stTRZ();
  r0_00 = (VECTOR *)local_5c;
  if (iVar12 != 0) {
    psVar13 = (short *)(&DAT_80073454 + (uint)*(ushort *)(param_3 + 0x9c) * 2);
    uVar8 = 0xffff0000;
    puVar2 = param_2 + 7;
    puVar3 = puVar6 + 2;
    do {
      gte_SetTransMatrix((MATRIX *)((int)r0_02 + 0x20));
      r0_00->vx = ((int)((uint)(ushort)r1->vx << 0x10) >> 0x15) * (int)*psVar13;
      r0_00->vy = (r1->vy + 300 >> 6) * (int)*psVar13;
      r0_00->vz = ((int)((uint)(ushort)r1->vz << 0x10) >> 0x15) * (int)*psVar13;
      gte_ldVXY0(CONCAT22((short)r0_00->vy,(short)r0_00->vx));
      gte_ldVZ0(&r0_00->vz);
      gte_rt();
      iVar12 = iVar12 + -1;
      gte_stlvnl(r0_00);
      gte_SetTransMatrix(r0_02);
      gte_ldv3(r0_01,r1,r2);
      gte_rtpt();
      puVar6 = puVar6 + 3;
      puVar4 = puVar3 + 3;
      r0_01 = (SVECTOR *)(iVar7 + ((*puVar6 & uVar8) >> 0xd));
      r1 = (SVECTOR *)(iVar7 + ((puVar3[2] & uVar8) >> 0xd));
      r2 = (SVECTOR *)(iVar7 + ((*puVar4 & uVar8) >> 0xd));
      gte_stsxy3_gt3(param_2);
      puVar2[-6] = uVar5;
      puVar2[-3] = uVar5;
      *puVar2 = uVar5;
      gte_avsz3();
      gte_stotz((long *)((int)r0_02 + 0x40));
      iVar1 = *(int *)((int)r0_02 + 0x40);
      if (iVar1 >> 6 != 0) {
        *(uint *)((int)r0_02 + 0x40) = *(uint *)(puVar10 + (iVar1 >> 4) * 4);
        *(uint *)(puVar10 + (iVar1 >> 4) * 4) = (uint)param_2 & uVar9;
        *param_2 = uVar9 & *(uint *)((int)r0_02 + 0x40) | uVar11;
      }
      puVar2 = puVar2 + 0x14;
      param_2 = param_2 + 0x14;
      puVar3 = puVar4;
    } while (iVar12 != 0);
    r0 = (MATRIX *)((int)r0_02 + 0x20);
  }
  gte_SetTransMatrix(r0);
  return;
}
