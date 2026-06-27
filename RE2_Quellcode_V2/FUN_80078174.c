/* FUN_80078174 @ 0x80078174  (Ghidra headless, raw MIPS overlay) */

void FUN_80078174(uint *param_1,int param_2,int param_3,int param_4,int param_5,int param_6,
                 undefined4 param_7,int param_8)

{
  byte bVar1;
  uint *puVar2;
  short sVar3;
  uint uVar4;
  short sVar5;
  uint uVar6;
  short sVar7;
  uint uVar8;
  short sVar9;
  uint uVar10;
  byte *pbVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  int iVar16;
  uint uVar17;
  SVECTOR *r0;
  int iVar18;
  undefined1 local_40 [4];
  undefined2 local_3c;
  undefined2 local_34;
  undefined2 local_2c;
  undefined2 local_24;
  
  r0 = (SVECTOR *)local_40;
  iVar18 = (uint)*(ushort *)(param_2 + 0x3a) * (param_5 >> 4);
  iVar16 = (int)(iVar18 * (uint)*(ushort *)(param_2 + 4)) >> 0xc;
  uVar15 = (uint)*(ushort *)(param_2 + 0x32) << 0x10;
  iVar12 = (int)(iVar18 * (uint)*(ushort *)(param_2 + 6)) >> 0xc;
  uVar14 = (uint)*(ushort *)(param_2 + 0x2a) << 0x10;
  local_3c = 0;
  local_34 = 0;
  local_2c = 0;
  local_24 = 0;
  pbVar11 = (byte *)(*(int *)(param_2 + 0x74) + param_8 * 4);
  iVar18 = param_3 + -1;
  uVar13 = (uint)(param_3 * iVar16) >> 0xc;
  uVar17 = (uint)(param_3 * iVar12) >> 0xc;
  while( true ) {
    bVar1 = pbVar11[3];
    sVar5 = (short)((char)pbVar11[2] * iVar16 >> 0xc);
    r0->vx = sVar5;
    sVar3 = (short)((char)bVar1 * iVar12 >> 0xc);
    r0->vy = sVar3;
    gte_ldv0(r0);
    sVar9 = sVar5 + (short)uVar13;
    sVar7 = sVar3 + (short)uVar17;
    gte_rtps_b();
    *(short *)((int)r0 + 8) = sVar9;
    *(short *)((int)r0 + 10) = sVar3;
    *(short *)((int)r0 + 0x10) = sVar5;
    *(short *)((int)r0 + 0x12) = sVar7;
    *(short *)((int)r0 + 0x18) = sVar9;
    *(short *)((int)r0 + 0x1a) = sVar7;
    uVar6 = (uint)*pbVar11;
    uVar4 = (uint)pbVar11[1] * 0x100;
    uVar10 = uVar6 + iVar18;
    uVar8 = uVar4 + iVar18 * 0x100;
    gte_stsxy((long *)(param_1 + 2));
    gte_stszotz((long *)((int)r0 + 0x20));
    gte_ldv3((SVECTOR *)((int)r0 + 8),(SVECTOR *)((int)r0 + 0x10),(SVECTOR *)((int)r0 + 0x18));
    if (0x1fff < *(uint *)((int)r0 + 0x20)) {
      *(undefined4 *)((int)r0 + 0x20) = 0x1fff;
    }
    gte_rtpt_b();
    if (*(uint *)((int)r0 + 0x20) >> 7 == 0) break;
    param_1[3] = uVar6 | uVar4 | uVar15;
    param_1[5] = uVar10 | uVar4 | uVar14;
    param_1[7] = uVar6 | uVar8;
    param_1[9] = uVar10 | uVar8;
    *(char *)((int)param_1 + 7) = (char)param_7;
    pbVar11 = pbVar11 + 4;
    puVar2 = (uint *)(param_6 + (*(uint *)((int)r0 + 0x20) >> 3) * 4);
    *param_1 = *puVar2 & 0xffffff | 0x9000000;
    *puVar2 = (uint)param_1 & 0xffffff;
    gte_stsxy3((long *)(param_1 + 4),(long *)(param_1 + 6),(long *)(param_1 + 8));
    param_4 = param_4 + -1;
    param_1 = param_1 + 10;
    if (param_4 == 0) {
      return;
    }
  }
  return;
}


