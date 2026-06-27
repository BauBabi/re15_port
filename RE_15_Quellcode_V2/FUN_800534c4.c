uint * FUN_800534c4(uint *param_1,int param_2,byte *param_3,int param_4,int param_5,int param_6)

{
  bool bVar1;
  byte bVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint *puVar10;
  byte *pbVar11;
  uint *puVar12;
  uint uVar13;
  int iVar14;
  int iVar15;
  uint uVar16;
  byte *pbVar17;
  uint *r0;
  uint uVar18;
  int iVar19;
  int iVar20;
  uint uVar21;
  uint uVar22;
  undefined1 local_48 [16];
  uint local_38;
  
  r0 = (uint *)local_48;
  local_38 = *(byte *)(param_2 + 0x6c) >> 3 & 2;
  gte_ldv0((SVECTOR *)(param_2 + 0x28));
  pbVar17 = (byte *)((uint)*param_3 * 4 + *(int *)(param_2 + 0x7c));
  gte_rtps();
  uVar18 = (uint)*(ushort *)(param_2 + 0x32) << 0x10;
  uVar16 = (uint)*(ushort *)(param_2 + 0x30) << 0x10;
  gte_stsxy((long *)r0);
  iVar15 = *r0 << 0x10;
  r0[6] = *r0 & 0xffff0000;
  gte_stsz((long *)(r0 + 1));
  uVar5 = r0[1];
  if ((int)uVar5 >> 2 != 0) {
    uVar13 = (uint)param_3[3];
    param_5 = uVar13 * *(ushort *)(param_2 + 0x72) * param_5;
    iVar4 = uVar5 << 4;
    iVar19 = param_5 / iVar4;
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (param_5 == -0x80000000)) {
      trap(0x1800);
    }
    iVar4 = (uint)*(ushort *)(param_2 + 4) * iVar19;
    iVar20 = (uint)*(ushort *)(param_2 + 6) * iVar19;
    uVar21 = iVar4 / (int)uVar13;
    if (uVar13 == 0) {
      trap(0x1c00);
    }
    if ((uVar13 == 0xffffffff) && (iVar4 == -0x80000000)) {
      trap(0x1800);
    }
    iVar14 = uVar13 * 0x100;
    puVar12 = (uint *)(param_6 + ((int)uVar5 >> 6) * 4);
    uVar5 = *puVar12;
    *r0 = iVar19;
    uVar22 = iVar20 / (int)uVar13;
    if (uVar13 == 0) {
      trap(0x1c00);
    }
    if ((uVar13 == 0xffffffff) && (iVar20 == -0x80000000)) {
      trap(0x1800);
    }
    r0[2] = uVar5;
    if (0x1ffff < uVar21) {
      uVar13 = uVar13 - 1;
    }
    if (0x1ffff < uVar22) {
      iVar14 = iVar14 + -0x100;
    }
    pbVar11 = pbVar17 + 1;
    puVar3 = param_1;
    do {
      puVar10 = puVar3;
      *(char *)((int)puVar10 + 7) = (char)r0[4] + ',';
      uVar6 = iVar15 + (int)(char)pbVar11[1] * uVar21;
      *r0 = uVar6;
      bVar2 = pbVar11[2];
      uVar7 = (uint)param_1 & 0xffffff;
      param_1 = param_1 + 10;
      uVar9 = iVar4 + uVar6 >> 0x10;
      uVar6 = uVar6 >> 0x10;
      *r0 = uVar6;
      uVar5 = r0[6] + (int)(char)bVar2 * uVar22;
      uVar8 = iVar20 + uVar5 & 0xffff0000;
      r0[1] = uVar5;
      uVar5 = uVar5 & 0xffff0000;
      r0[1] = uVar5;
      puVar10[4] = uVar9 | uVar5;
      puVar10[2] = uVar6 | uVar5;
      puVar10[6] = uVar8 | uVar6;
      puVar10[8] = uVar9 | uVar8;
      uVar6 = (uint)*pbVar17;
      pbVar17 = pbVar17 + 4;
      *r0 = uVar6;
      bVar2 = *pbVar11;
      pbVar11 = pbVar11 + 4;
      uVar5 = (uint)bVar2 * 0x100;
      r0[1] = uVar5;
      puVar10[3] = uVar6 | uVar5 | uVar18;
      puVar10[5] = uVar13 + uVar6 | uVar5 | uVar16;
      puVar10[7] = iVar14 + uVar5 | uVar6;
      puVar10[9] = uVar13 + uVar6 | iVar14 + uVar5;
      *puVar12 = *puVar12 & 0xff000000 | uVar7;
      bVar1 = param_4 != 0;
      puVar3 = puVar10 + 10;
      puVar12 = puVar10;
      param_4 = param_4 + -1;
    } while (bVar1);
    *puVar10 = *puVar10 & 0xff000000 | r0[2] & 0xffffff;
  }
  return param_1;
}
