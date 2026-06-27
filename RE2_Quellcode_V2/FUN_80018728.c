/* FUN_80018728 @ 0x80018728  (Ghidra headless, raw MIPS overlay) */

void FUN_80018728(int param_1,MATRIX *param_2,int param_3,short param_4,undefined4 param_5,
                 short param_6,undefined4 param_7,undefined2 param_8,short param_9,
                 undefined4 param_10,undefined4 param_11,undefined4 param_12,undefined4 param_13)

{
  undefined4 uVar1;
  undefined4 uVar2;
  long lVar3;
  long lVar4;
  long lVar5;
  short sVar6;
  ushort uVar7;
  ushort uVar8;
  uint uVar9;
  MATRIX *pMVar10;
  int iVar11;
  undefined4 uVar12;
  int iVar13;
  VECTOR *_3;
  undefined1 *puVar14;
  int iVar15;
  SVECTOR *r;
  undefined1 *puVar16;
  undefined1 auStack_120 [16];
  MATRIX local_110;
  SVECTOR local_f0 [15];
  undefined4 local_78;
  undefined4 local_70;
  undefined4 local_68;
  undefined4 local_60;
  undefined4 local_58;
  undefined2 local_50;
  undefined4 local_48;
  int local_40;
  
  puVar16 = auStack_120;
  r = local_f0;
  local_78 = param_5;
  local_70 = param_7;
  local_68 = param_10;
  local_60 = param_11;
  local_58 = param_12;
  local_f0[0]._0_4_ = (uint)*(ushort *)(param_1 + 0x68) + (uint)*(ushort *)(param_1 + 0x9a) & 0xffff
  ;
  local_48 = param_13;
  local_f0[0].vz = *(short *)(param_1 + 0x6c) + *(short *)(param_1 + 0x98);
  RotMatrixYXZ(r,&local_110);
  RotMatrixY((int)*(short *)(param_1 + 0x6a),&local_110);
  local_f0[0]._0_4_ = param_3 << 0x10;
  pMVar10 = &local_110;
  local_f0[0].vz = 0;
  local_50 = (undefined2)local_48;
  local_40 = param_1 + 0x48;
  gte_SetRotMatrix(pMVar10);
  gte_ldv0(r);
  gte_rtv0();
  gte_stlvnl((VECTOR *)(puVar16 + 0x38));
  *(undefined2 *)(puVar16 + 0x32) = param_8;
  gte_ldv0(r);
  gte_rtv0();
  *(int *)(puVar16 + 0x38) = *(int *)(puVar16 + 0x38) + *(int *)(param_1 + 0x5c);
  *(int *)(puVar16 + 0x3c) = *(int *)(puVar16 + 0x3c) + *(int *)(param_1 + 0x60) + (int)param_4;
  *(int *)(puVar16 + 0x40) = *(int *)(puVar16 + 0x40) + *(int *)(param_1 + 100);
  gte_stlvnl((VECTOR *)(puVar16 + 0x48));
  iVar11 = DAT_800cfd90;
  puVar14 = puVar16 + 0x68;
  FUN_8002ce94(&DAT_800cfc1c,DAT_800cfd90 + 0x18,puVar14);
  iVar13 = iVar11 + 0x5a8;
  FUN_8002ce94(puVar14,iVar11 + 0x578,iVar13);
  uVar2 = DAT_8009db50;
  uVar1 = DAT_8009db4c;
  uVar12 = DAT_8009db48;
  *(undefined4 *)(puVar16 + 0x68) = DAT_8009db44;
  *(undefined4 *)(puVar16 + 0x6c) = uVar12;
  *(undefined4 *)(puVar16 + 0x70) = uVar1;
  *(undefined4 *)(puVar16 + 0x74) = uVar2;
  lVar5 = DAT_8009db60;
  lVar4 = DAT_8009db5c;
  lVar3 = DAT_8009db58;
  *(undefined4 *)(puVar16 + 0x78) = DAT_8009db54;
  *(long *)(puVar16 + 0x7c) = lVar3;
  *(long *)(puVar16 + 0x80) = lVar4;
  *(long *)(puVar16 + 0x84) = lVar5;
  *(undefined4 *)(puVar16 + 0x7c) = *(undefined4 *)(puVar16 + 0x38);
  *(undefined4 *)(puVar16 + 0x80) = *(undefined4 *)(puVar16 + 0x3c);
  *(undefined4 *)(puVar16 + 0x84) = *(undefined4 *)(puVar16 + 0x40);
  FUN_8002d030(iVar13,puVar14,puVar16 + 0x88);
  *(int *)(puVar16 + 0x48) = *(int *)(puVar16 + 0x48) + *(int *)(param_1 + 0x5c);
  _3 = (VECTOR *)(puVar16 + 0x58);
  *(int *)(puVar16 + 0x4c) = *(int *)(puVar16 + 0x4c) + *(int *)(param_1 + 0x60) + (int)param_9;
  *(int *)(puVar16 + 0x50) = *(int *)(puVar16 + 0x50) + *(int *)(param_1 + 100);
  iVar11 = *(int *)(param_1 + 0x34);
  *(uint *)(puVar16 + 0x30) =
       (uint)*(ushort *)(param_1 + 0x2c) << 4 | (uint)*(ushort *)(param_1 + 0x30) << 0x14;
  *(short *)(puVar16 + 0x34) = (short)(iVar11 << 4);
  ApplyMatrix(param_2,r,_3);
  *(int *)(param_1 + 0x5c) = (*(int *)(puVar16 + 0x58) >> 4) + param_2->t[0];
  *(int *)(param_1 + 0x60) = (*(int *)(puVar16 + 0x5c) >> 4) + param_2->t[1];
  *(int *)(param_1 + 100) = (*(int *)(puVar16 + 0x60) >> 4) + param_2->t[2];
  uVar2 = DAT_8009db50;
  uVar1 = DAT_8009db4c;
  uVar12 = DAT_8009db48;
  *(undefined4 *)(puVar16 + 0x10) = DAT_8009db44;
  *(undefined4 *)(puVar16 + 0x14) = uVar12;
  *(undefined4 *)(puVar16 + 0x18) = uVar1;
  *(undefined4 *)(puVar16 + 0x1c) = uVar2;
  lVar5 = DAT_8009db60;
  lVar4 = DAT_8009db5c;
  lVar3 = DAT_8009db58;
  *(undefined4 *)(puVar16 + 0x20) = DAT_8009db54;
  *(long *)(puVar16 + 0x24) = lVar3;
  *(long *)(puVar16 + 0x28) = lVar4;
  *(long *)(puVar16 + 0x2c) = lVar5;
  RotMatrixY(-(int)*(short *)(param_1 + 0x6a),pMVar10);
  *(uint *)(puVar16 + 0x30) =
       (*(int *)(param_1 + 0x60) - *(int *)(puVar16 + 0x3c)) * 0x40000 |
       (*(int *)(param_1 + 0x5c) - *(int *)(puVar16 + 0x38) & 0xffffU) << 2;
  *(short *)(puVar16 + 0x34) =
       ((short)*(undefined4 *)(param_1 + 100) - (short)*(undefined4 *)(puVar16 + 0x40)) * 4;
  ApplyMatrix(pMVar10,r,_3);
  sVar6 = FUN_80018cc8((int)*(short *)(puVar16 + 0x58),(int)*(short *)(puVar16 + 0x5c));
  *(uint *)(puVar16 + 0x30) =
       (*(int *)(param_1 + 0x60) - *(int *)(puVar16 + 0x4c)) * 0x40000 |
       (*(int *)(param_1 + 0x5c) - *(int *)(puVar16 + 0x48) & 0xffffU) << 2;
  *(short *)(puVar16 + 0x34) =
       ((short)*(undefined4 *)(param_1 + 100) - (short)*(undefined4 *)(puVar16 + 0x50)) * 4;
  RotMatrixZ(-((int)*(short *)(param_1 + 0x6c) + (int)*(short *)(param_1 + 0x98)),pMVar10);
  ApplyMatrix(pMVar10,r,_3);
  uVar7 = FUN_80018d64((int)(short)-(short)*(undefined4 *)(puVar16 + 0x60),
                       (int)*(short *)(puVar16 + 0x5c));
  iVar11 = rsin((int)sVar6 - (int)(short)*(undefined4 *)(puVar16 + 0xb0));
  iVar13 = (int)*(short *)(param_1 + 0x98);
  iVar15 = (int)param_6;
  if (iVar15 == 0) {
    trap(0x1c00);
  }
  if ((iVar15 == -1) && (iVar13 == -0x80000000)) {
    trap(0x1800);
  }
  uVar9 = (iVar13 - iVar13 / iVar15) - ((short)*(undefined4 *)(puVar16 + 0xa8) * iVar11 >> 0xc);
  uVar8 = (ushort)uVar9 & 0xfff;
  *(ushort *)(param_1 + 0x98) = uVar8;
  if ((uVar9 & 0x800) != 0) {
    *(ushort *)(param_1 + 0x98) = uVar8 - 0x1000;
  }
  iVar11 = rsin((int)(short)uVar7 - (int)(short)*(undefined4 *)(puVar16 + 200));
  iVar13 = (int)*(short *)(param_1 + 0x9a);
  iVar15 = (int)(short)*(undefined4 *)(puVar16 + 0xc0);
  if (iVar15 == 0) {
    trap(0x1c00);
  }
  if ((iVar15 == -1) && (iVar13 == -0x80000000)) {
    trap(0x1800);
  }
  uVar9 = (iVar13 - iVar13 / iVar15) - ((short)*(undefined4 *)(puVar16 + 0xb8) * iVar11 >> 0xc);
  uVar8 = (ushort)uVar9 & 0xfff;
  *(ushort *)(param_1 + 0x9a) = uVar8;
  if ((uVar9 & 0x800) != 0) {
    *(ushort *)(param_1 + 0x9a) = uVar8 - 0x1000;
  }
  pMVar10 = *(MATRIX **)(puVar16 + 0xe0);
  *(short *)(param_1 + 0x6c) = sVar6;
  *(uint *)(param_1 + 0x68) = (uint)uVar7;
  RotMatrixYXZ((SVECTOR *)(param_1 + 0x68),pMVar10);
  uVar12 = *(undefined4 *)(puVar16 + 0xd8);
  pMVar10 = *(MATRIX **)(puVar16 + 0xe0);
  *(undefined2 *)(param_1 + 0x6a) = *(undefined2 *)(puVar16 + 0xd0);
  RotMatrixY((int)(short)uVar12,pMVar10);
  return;
}


