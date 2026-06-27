/* FUN_8004baf0 @ 0x8004baf0  (Ghidra headless, raw MIPS overlay) */

void FUN_8004baf0(int param_1,int param_2)

{
  byte bVar1;
  ushort uVar2;
  short *psVar3;
  short *psVar4;
  short *psVar5;
  short *psVar6;
  int iVar7;
  SVECTOR *r0;
  SVECTOR *pSVar8;
  VECTOR *r0_00;
  int iVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  ushort *puVar13;
  uint uVar14;
  uint uVar15;
  int *piVar16;
  byte *pbVar17;
  byte *pbVar18;
  uint uVar19;
  int iVar20;
  int *piVar21;
  int iVar22;
  uint unaff_s2;
  uint uVar23;
  uint uVar24;
  SVECTOR *r0_01;
  int iVar25;
  undefined1 local_38 [8];
  uint local_30;
  
  r0_01 = (SVECTOR *)local_38;
  piVar21 = *(int **)(param_2 + 0x88);
  iVar20 = *(int *)(param_1 + 0x1bc);
  iVar11 = (*(int **)(param_2 + 8))[1];
  r0 = (SVECTOR *)**(int **)(param_2 + 8);
  psVar3 = (short *)(*piVar21 + iVar20);
  psVar6 = &r0->vz;
  psVar5 = psVar3;
  pSVar8 = r0;
  iVar12 = iVar11;
  do {
    iVar12 = iVar12 + -1;
    pSVar8->vx = *psVar5;
    psVar4 = psVar5 + 2;
    pSVar8 = pSVar8 + 1;
    psVar6[-1] = psVar5[1];
    psVar5 = psVar5 + 3;
    *psVar6 = *psVar4;
    psVar6 = psVar6 + 4;
  } while (iVar12 != 0);
  puVar13 = *(ushort **)(param_1 + 0x108);
  uVar15 = (uint)*(byte *)(param_2 + 0x78);
  uVar24 = (uint)*(ushort *)(piVar21 + 2);
  iVar12 = *(int *)(param_1 + 0x198);
  uVar23 = (uint)piVar21[1] >> 1;
  uVar2 = puVar13[2];
  uVar19 = *(uint *)((int)puVar13 + uVar15 * 4 + (*puVar13 & 0xfffc));
  pbVar17 = (byte *)((int)puVar13 + (uVar19 >> 0x10) + (*puVar13 & 0xfffc));
  iVar22 = *(int *)(iVar20 + 4) + iVar20;
  if (uVar15 < uVar2) {
    unaff_s2 = uVar19 & 0xffff;
    pbVar18 = pbVar17;
    for (uVar19 = unaff_s2; uVar19 != 0; uVar19 = uVar19 - 1) {
      bVar1 = *pbVar18;
      pbVar18 = pbVar18 + 1;
      psVar5 = (short *)((uint)bVar1 * 6 + iVar22);
      iVar9 = iVar12 + (uint)bVar1 * 0xac;
      *(int *)(iVar9 + 0x2c) = (int)*psVar5;
      *(int *)(iVar9 + 0x30) = (int)psVar5[1];
      *(int *)(iVar9 + 0x34) = (int)psVar5[2];
    }
  }
  local_30 = (uint)((uint)uVar2 < uVar15 + 1);
  iVar20 = *(int *)(iVar20 + 8);
  uVar15 = uVar23;
  do {
    piVar16 = piVar21 + 1;
    uVar19 = (uint)*(ushort *)(piVar21 + 3);
    iVar9 = 0xfff - uVar19;
    pSVar8 = r0;
    iVar25 = iVar11;
    if (uVar19 != 0) {
      psVar5 = psVar3 + uVar23;
      do {
        gte_ldIR0(iVar9);
        gte_ldsv(r0);
        r0_01->vx = *psVar5;
        gte_gpf12_b();
        psVar6 = psVar5 + 2;
        r0_01->vy = psVar5[1];
        r0_01->vz = *psVar6;
        gte_ldIR0(uVar19);
        gte_ldsv(r0_01);
        iVar11 = iVar11 + -1;
        psVar5 = psVar6 + 1;
        gte_gpl12_b();
        gte_stsv(r0);
        r0 = r0 + 1;
      } while (iVar11 != 0);
      if (*(int *)((int)r0_01 + 8) == 0) {
        uVar10 = (uint)*pbVar17;
        iVar11 = iVar22 + iVar20 * (uint)*(ushort *)((int)piVar16 + 10);
        uVar14 = unaff_s2;
        pbVar18 = pbVar17;
        while (uVar14 != 0) {
          uVar14 = uVar14 - 1;
          iVar7 = uVar10 * 3;
          gte_ldIR0(iVar9);
          r0_00 = (VECTOR *)(iVar12 + uVar10 * 0xac + 0x2c);
          gte_ldlvl(r0_00);
          gte_gpf12();
          psVar5 = (short *)(iVar7 * 2 + iVar11);
          r0_01->vx = *psVar5;
          r0_01->vy = psVar5[1];
          r0_01->vz = psVar5[2];
          gte_ldIR0(uVar19);
          gte_ldsv(r0_01);
          pbVar18 = pbVar18 + 1;
          uVar10 = (uint)*pbVar18;
          gte_gpl12_b();
          gte_stlvl(r0_00);
        }
      }
    }
    uVar24 = uVar24 - 1;
    uVar23 = uVar23 + uVar15;
    r0 = pSVar8;
    iVar11 = iVar25;
    piVar21 = piVar16;
  } while (uVar24 != 0);
  return;
}


