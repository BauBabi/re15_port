/* FUN_800197f4 @ 0x800197f4  (Ghidra headless, raw MIPS overlay) */

void FUN_800197f4(int param_1,int param_2,byte *param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  SVECTOR *pSVar5;
  int iVar6;
  uint uVar7;
  ushort *puVar8;
  undefined4 uVar9;
  SVECTOR *pSVar10;
  SVECTOR *pSVar11;
  SVECTOR *pSVar12;
  uint uVar13;
  uint uVar14;
  SVECTOR *pSVar15;
  int *piVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  
  pSVar12 = *(SVECTOR **)(param_3 + 0xc);
  pSVar10 = *(SVECTOR **)(param_3 + 4);
  pSVar15 = *(SVECTOR **)(param_3 + 0x10);
  uVar13 = (uint)*(ushort *)(param_3 + 2);
  piVar16 = *(int **)(param_2 + 8);
  iVar2 = *(int *)(param_1 + 0x198) + (uint)*param_3 * 0xac;
  pSVar11 = (SVECTOR *)(*(int *)(param_3 + 8) + -8);
  uVar14 = uVar13;
  gte_SetRotMatrix((MATRIX *)(iVar2 + 0x48));
  iVar6 = 0;
  while( true ) {
    uVar18 = *(undefined4 *)(iVar2 + 0x5c);
    uVar17 = *(undefined4 *)(iVar2 + 0x60);
    uVar9 = *(undefined4 *)(iVar2 + 100);
    do {
      gte_ldv0(pSVar12);
      pSVar11 = pSVar11 + 1;
      pSVar12 = pSVar12 + 1;
      gte_rtv0_b();
      gte_stsv(pSVar10);
      gte_ldv0(pSVar15);
      pSVar10->vx = pSVar10->vx + (short)uVar18;
      gte_rtv0_b();
      pSVar15 = pSVar15 + 1;
      pSVar10->vy = pSVar10->vy + (short)uVar17;
      pSVar10->vz = pSVar10->vz + (short)uVar9;
      pSVar10 = pSVar10 + 1;
      gte_stsv(pSVar11);
      uVar14 = uVar14 - 1;
    } while (uVar14 != 0);
    if (iVar6 != 0) break;
    pSVar12 = *(SVECTOR **)(param_3 + 0x24);
    pSVar10 = *(SVECTOR **)(param_3 + 0x1c);
    pSVar15 = *(SVECTOR **)(param_3 + 0x10);
    iVar2 = *(int *)(param_1 + 0x198) + (uint)param_3[1] * 0xac;
    pSVar11 = (SVECTOR *)(*(int *)(param_3 + 0x20) + -8);
    uVar14 = uVar13;
    gte_SetRotMatrix((MATRIX *)(iVar2 + 0x48));
    iVar6 = 1;
  }
  pSVar15 = *(SVECTOR **)(param_3 + 4);
  pSVar11 = *(SVECTOR **)(param_3 + 0x1c);
  iVar2 = *piVar16;
  pSVar12 = *(SVECTOR **)(param_3 + 8);
  pSVar10 = *(SVECTOR **)(param_3 + 0x20);
  iVar6 = piVar16[2];
  puVar8 = *(ushort **)(param_3 + 0x34);
  do {
    iVar1 = 0x1000;
    uVar7 = (uint)puVar8[1];
    uVar14 = (uint)*puVar8;
    if (uVar7 == 0x1000) {
      puVar4 = (undefined4 *)(uVar14 * 8 + iVar2);
      *puVar4 = *(undefined4 *)pSVar15;
      puVar4[1] = *(undefined4 *)&pSVar15->vz;
      puVar4 = (undefined4 *)(uVar14 * 8 + iVar6);
      *puVar4 = *(undefined4 *)pSVar12;
      puVar4[1] = *(undefined4 *)&pSVar12->vz;
LAB_80019a2c:
      puVar8 = puVar8 + 2;
      pSVar15 = pSVar15 + 1;
      pSVar11 = pSVar11 + 1;
      pSVar12 = pSVar12 + 1;
      pSVar10 = pSVar10 + 1;
      uVar13 = uVar13 - 1;
    }
    else {
      if (uVar7 == 0) {
        puVar4 = (undefined4 *)(uVar14 * 8 + iVar2);
        *puVar4 = *(undefined4 *)pSVar11;
        puVar4[1] = *(undefined4 *)&pSVar11->vz;
        puVar4 = (undefined4 *)(uVar14 * 8 + iVar6);
        *puVar4 = *(undefined4 *)pSVar10;
        puVar4[1] = *(undefined4 *)&pSVar10->vz;
        goto LAB_80019a2c;
      }
      gte_ldIR0(uVar7);
      gte_ldsv(pSVar15);
      iVar3 = uVar14 * 8;
      pSVar5 = (SVECTOR *)(iVar2 + iVar3);
      gte_gpf12_b();
      iVar1 = iVar1 - uVar7;
      gte_ldIR0(iVar1);
      gte_ldsv(pSVar11);
      puVar8 = puVar8 + 2;
      uVar13 = uVar13 - 1;
      gte_gpl12_b();
      gte_stsv(pSVar5);
      gte_ldIR0(uVar7);
      gte_ldsv(pSVar12);
      pSVar15 = pSVar15 + 1;
      pSVar11 = pSVar11 + 1;
      gte_gpf12_b();
      pSVar5 = (SVECTOR *)(iVar6 + iVar3);
      gte_ldIR0(iVar1);
      gte_ldsv(pSVar10);
      pSVar12 = pSVar12 + 1;
      pSVar10 = pSVar10 + 1;
      gte_gpl12_b();
      gte_stsv(pSVar5);
    }
    if (uVar13 == 0) {
      return;
    }
  } while( true );
}


