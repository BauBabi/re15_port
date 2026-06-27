void FUN_80024e40(int param_1,MATRIX *param_2,int param_3,short param_4,undefined4 param_5,
                 short param_6,undefined4 param_7,undefined2 param_8,short param_9,
                 undefined4 param_10,undefined4 param_11,undefined4 param_12,undefined4 param_13)

{
  short sVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  ushort uVar5;
  MATRIX *pMVar6;
  int iVar7;
  undefined4 uVar8;
  VECTOR *$3;
  int iVar9;
  SVECTOR *r;
  undefined1 *puVar10;
  undefined1 auStack_f0 [16];
  MATRIX local_e0;
  SVECTOR local_c0 [7];
  undefined4 local_88;
  undefined4 local_80;
  undefined4 local_78;
  undefined4 local_70;
  undefined4 local_68;
  undefined2 local_60;
  undefined4 local_58;
  int local_50;
  
  puVar10 = auStack_f0;
  r = local_c0;
  local_88 = param_5;
  local_80 = param_7;
  local_78 = param_10;
  local_70 = param_11;
  local_68 = param_12;
  local_c0[0]._0_4_ = (uint)*(ushort *)(param_1 + 0x60) + (uint)*(ushort *)(param_1 + 0x96) & 0xffff
  ;
  local_58 = param_13;
  local_c0[0].vz = *(short *)(param_1 + 100) + *(short *)(param_1 + 0x94);
  RotMatrixYXZ(r,&local_e0);
  RotMatrixY((int)*(short *)(param_1 + 0x62),&local_e0);
  local_c0[0]._0_4_ = param_3 << 0x10;
  pMVar6 = &local_e0;
  local_c0[0].vz = 0;
  local_60 = (undefined2)local_58;
  local_50 = param_1 + 0x40;
  gte_SetRotMatrix(pMVar6);
  gte_ldv0(r);
  gte_rtv0();
  gte_stlvnl((VECTOR *)(puVar10 + 0x38));
  *(undefined2 *)(puVar10 + 0x32) = param_8;
  gte_ldv0(r);
  gte_rtv0();
  *(int *)(puVar10 + 0x38) = *(int *)(puVar10 + 0x38) + *(int *)(param_1 + 0x54);
  *(int *)(puVar10 + 0x3c) = (int)param_4 + *(int *)(param_1 + 0x58) + *(int *)(puVar10 + 0x3c);
  *(int *)(puVar10 + 0x40) = *(int *)(puVar10 + 0x40) + *(int *)(param_1 + 0x5c);
  gte_stlvnl((VECTOR *)(puVar10 + 0x48));
  $3 = (VECTOR *)(puVar10 + 0x58);
  *(int *)(puVar10 + 0x48) = *(int *)(puVar10 + 0x48) + *(int *)(param_1 + 0x54);
  *(int *)(puVar10 + 0x4c) = (int)param_9 + *(int *)(param_1 + 0x58) + *(int *)(puVar10 + 0x4c);
  *(int *)(puVar10 + 0x50) = *(int *)(puVar10 + 0x50) + *(int *)(param_1 + 0x5c);
  sVar1 = *(short *)(param_1 + 0x34);
  *(uint *)(puVar10 + 0x30) =
       (uint)*(ushort *)(param_1 + 0x2c) << 4 | (uint)*(ushort *)(param_1 + 0x30) << 0x14;
  *(short *)(puVar10 + 0x34) = sVar1 << 4;
  ApplyMatrix(param_2,r,$3);
  *(int *)(param_1 + 0x54) = (*(int *)(puVar10 + 0x58) >> 4) + param_2->t[0];
  *(int *)(param_1 + 0x58) = (*(int *)(puVar10 + 0x5c) >> 4) + param_2->t[1];
  *(int *)(param_1 + 0x5c) = (*(int *)(puVar10 + 0x60) >> 4) + param_2->t[2];
  *(undefined4 *)(puVar10 + 0x10) = 0x1000;
  *(undefined4 *)(puVar10 + 0x14) = 0;
  *(undefined4 *)(puVar10 + 0x18) = 0x1000;
  *(undefined4 *)(puVar10 + 0x1c) = 0;
  *(undefined4 *)(puVar10 + 0x20) = 0x1000;
  *(undefined4 *)(puVar10 + 0x24) = 0;
  *(undefined4 *)(puVar10 + 0x28) = 0;
  *(undefined4 *)(puVar10 + 0x2c) = 0;
  RotMatrixY(-(int)*(short *)(param_1 + 0x62),pMVar6);
  *(uint *)(puVar10 + 0x30) =
       ((uint)*(ushort *)(param_1 + 0x58) - (uint)*(ushort *)(puVar10 + 0x3c)) * 0x40000 |
       ((uint)*(ushort *)(param_1 + 0x54) - (uint)*(ushort *)(puVar10 + 0x38) & 0xffff) << 2;
  *(short *)(puVar10 + 0x34) = (*(short *)(param_1 + 0x5c) - *(short *)(puVar10 + 0x40)) * 4;
  ApplyMatrix(pMVar6,r,$3);
  sVar1 = FUN_80025360((int)*(short *)(puVar10 + 0x58),(int)*(short *)(puVar10 + 0x5c));
  *(uint *)(puVar10 + 0x30) =
       ((uint)*(ushort *)(param_1 + 0x58) - (uint)*(ushort *)(puVar10 + 0x4c)) * 0x40000 |
       ((uint)*(ushort *)(param_1 + 0x54) - (uint)*(ushort *)(puVar10 + 0x48) & 0xffff) << 2;
  *(short *)(puVar10 + 0x34) = (*(short *)(param_1 + 0x5c) - *(short *)(puVar10 + 0x50)) * 4;
  RotMatrixZ(-((int)*(short *)(param_1 + 100) + (int)*(short *)(param_1 + 0x94)),pMVar6);
  ApplyMatrix(pMVar6,r,$3);
  uVar2 = FUN_800253fc((int)(short)-(short)*(undefined4 *)(puVar10 + 0x60),
                       (int)*(short *)(puVar10 + 0x5c));
  *(int *)(puVar10 + 0xb0) = *(int *)(puVar10 + 0x70) << 0x10;
  iVar7 = (*(int *)(puVar10 + 0x70) << 0x10) >> 0x10;
  *(int *)(puVar10 + 0xa8) = iVar7;
  iVar7 = rsin(sVar1 - iVar7);
  iVar3 = (int)*(short *)(param_1 + 0x94);
  iVar9 = (int)param_6;
  if (iVar9 == 0) {
    trap(0x1c00);
  }
  if ((iVar9 == -1) && (iVar3 == -0x80000000)) {
    trap(0x1800);
  }
  uVar4 = (iVar3 - iVar3 / iVar9) - ((short)*(undefined4 *)(puVar10 + 0x68) * iVar7 >> 0xc);
  uVar5 = (ushort)uVar4 & 0xfff;
  *(ushort *)(param_1 + 0x94) = uVar5;
  if ((uVar4 & 0x800) != 0) {
    *(ushort *)(param_1 + 0x94) = uVar5 - 0x1000;
  }
  iVar7 = rsin((int)(short)uVar2 - (int)(short)*(undefined4 *)(puVar10 + 0x88));
  iVar3 = (int)*(short *)(param_1 + 0x96);
  iVar9 = (int)(short)*(undefined4 *)(puVar10 + 0x80);
  if (iVar9 == 0) {
    trap(0x1c00);
  }
  if ((iVar9 == -1) && (iVar3 == -0x80000000)) {
    trap(0x1800);
  }
  uVar4 = (iVar3 - iVar3 / iVar9) - ((short)*(undefined4 *)(puVar10 + 0x78) * iVar7 >> 0xc);
  uVar5 = (ushort)uVar4 & 0xfff;
  *(ushort *)(param_1 + 0x96) = uVar5;
  if ((uVar4 & 0x800) != 0) {
    *(ushort *)(param_1 + 0x96) = uVar5 - 0x1000;
  }
  pMVar6 = *(MATRIX **)(puVar10 + 0xa0);
  *(short *)(param_1 + 100) = sVar1;
  *(uint *)(param_1 + 0x60) = (uint)uVar2;
  RotMatrixYXZ((SVECTOR *)(param_1 + 0x60),pMVar6);
  uVar8 = *(undefined4 *)(puVar10 + 0x98);
  pMVar6 = *(MATRIX **)(puVar10 + 0xa0);
  *(undefined2 *)(param_1 + 0x62) = *(undefined2 *)(puVar10 + 0x90);
  RotMatrixY((int)(short)uVar8,pMVar6);
  return;
}
