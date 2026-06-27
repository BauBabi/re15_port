/* FUN_80077ed0 @ 0x80077ed0  (Ghidra headless, raw MIPS overlay) */

void FUN_80077ed0(uint *param_1,int param_2,uint param_3,int param_4,int param_5,int param_6,
                 undefined1 param_7,int param_8)

{
  byte *pbVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  uint uVar9;
  uint uVar10;
  uint *puVar11;
  byte *pbVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  uint *r0;
  uint uVar16;
  uint uVar17;
  uint uVar18;
  undefined1 local_18 [8];
  
  r0 = (uint *)local_18;
  gte_ldv0((SVECTOR *)(param_2 + 0x34));
  iVar13 = param_3 << 8;
  gte_rtps_b();
  param_5 = param_3 * *(ushort *)(param_2 + 0x3a) * param_5;
  ((long *)r0)[1] = param_4;
  pbVar12 = (byte *)(*(int *)(param_2 + 0x74) + param_8 * 4);
  uVar15 = (uint)*(ushort *)(param_2 + 0x32) << 0x10;
  uVar14 = (uint)*(ushort *)(param_2 + 0x2a) << 0x10;
  gte_stsz((long *)r0);
  if ((int)*r0 >> 9 != 0) {
    if (0x7fff < *r0) {
      *r0 = 0x7fff;
    }
    puVar11 = (uint *)(param_6 + ((int)*r0 >> 5) * 4);
    puVar8 = param_1;
    do {
      *puVar8 = *puVar11 & 0xffffff | 0x9000000;
      *puVar11 = (uint)puVar8 & 0xffffff;
      uVar3 = r0[1];
      puVar8 = puVar8 + 10;
      r0[1] = uVar3 - 1;
    } while (uVar3 - 1 != 0);
    gte_stsxy((long *)(r0 + 1));
    iVar4 = *r0 << 4;
    uVar3 = param_5 / iVar4;
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (param_5 == -0x80000000)) {
      trap(0x1800);
    }
    uVar16 = uVar3 * *(ushort *)(param_2 + 4);
    uVar17 = uVar16 / param_3;
    if (param_3 == 0) {
      trap(0x1c00);
    }
    uVar18 = uVar3 * *(ushort *)(param_2 + 6);
    uVar6 = r0[1];
    r0[1] = uVar3;
    uVar3 = uVar18 / param_3;
    if (param_3 == 0) {
      trap(0x1c00);
    }
    if (0x1ffff < uVar17) {
      param_3 = param_3 - 1;
    }
    if (0x1ffff < uVar3) {
      iVar13 = iVar13 + -0x100;
    }
    do {
      *(undefined1 *)((int)param_1 + 7) = param_7;
      bVar2 = pbVar12[3];
      uVar7 = uVar6 * 0x10000 + (int)(char)pbVar12[2] * uVar17;
      uVar10 = uVar7 + uVar16 >> 0x10;
      r0[1] = uVar7;
      uVar7 = uVar7 >> 0x10;
      r0[1] = uVar7;
      uVar5 = (uVar6 & 0xffff0000) + (int)(char)bVar2 * uVar3;
      uVar9 = uVar5 + uVar18 & 0xffff0000;
      *r0 = uVar5;
      uVar5 = uVar5 & 0xffff0000;
      *r0 = uVar5;
      param_1[4] = uVar10 | uVar5;
      param_1[2] = uVar7 | uVar5;
      param_1[6] = uVar7 | uVar9;
      param_1[8] = uVar10 | uVar9;
      uVar7 = (uint)*pbVar12;
      param_4 = param_4 + -1;
      r0[1] = uVar7;
      pbVar1 = pbVar12 + 1;
      pbVar12 = pbVar12 + 4;
      uVar5 = (uint)*pbVar1 * 0x100;
      *r0 = uVar5;
      param_1[3] = uVar7 | uVar5 | uVar15;
      param_1[5] = uVar7 + param_3 | uVar5 | uVar14;
      param_1[7] = uVar7 | uVar5 + iVar13;
      param_1[9] = uVar7 + param_3 | uVar5 + iVar13;
      param_1 = param_1 + 10;
    } while (param_4 != 0);
  }
  return;
}


