void FUN_80016b54(int param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  byte *pbVar12;
  uint uVar13;
  uint *r0;
  uint *puVar14;
  uint *unaff_s2;
  undefined *puVar15;
  short *psVar16;
  short *psVar17;
  int iVar18;
  int iVar19;
  SVECTOR *r0_00;
  undefined1 *puVar20;
  ulonglong uVar21;
  undefined1 auStack_98 [80];
  undefined4 local_48;
  
  puVar20 = auStack_98;
  pbVar12 = &DAT_800aca34;
  piVar6 = *(int **)(param_1 + 4);
  r0 = (uint *)((uint)DAT_800aca34 * 0x28 + *(int *)(param_1 + 8));
  iVar19 = piVar6[5];
  iVar18 = *piVar6;
  iVar11 = piVar6[4];
                    /* Possible PsyQ macro: setPolyGT3() */
  *(undefined1 *)(param_1 + 0x67) = 0x34;
  local_48 = param_2;
  gte_ldrgb((CVECTOR *)(param_1 + 100));
  sVar2 = *(short *)(iVar11 + 6);
  *(undefined4 *)(puVar20 + 0x58) = 0xffffff;
  sVar3 = *(short *)(iVar11 + 10);
  *(int *)(puVar20 + 0x60) = iVar18 + sVar2 * 8;
  sVar2 = *(short *)(iVar11 + 2);
  iVar7 = iVar18 + sVar3 * 8;
  *(int *)(puVar20 + 0x68) = iVar7;
  iVar4 = sVar2 * 8;
  uVar21 = CONCAT44(iVar7,iVar4);
  r0_00 = (SVECTOR *)(iVar18 + iVar4);
  if (iVar19 != 0) {
    puVar14 = r0 + 7;
    psVar16 = (short *)(iVar11 + 2);
    do {
      gte_ldv3(r0_00,*(SVECTOR **)(puVar20 + 0x60),*(SVECTOR **)(puVar20 + 0x68));
      gte_rtpt();
      puVar1 = (undefined1 *)((int)&r0_00->vy + 1);
      uVar10 = (uint)puVar1 & 3;
      uVar9 = (uint)r0_00 & 3;
      uVar5 = (*(int *)(puVar1 + -uVar10) << (3 - uVar10) * 8 |
              (uint)uVar21 & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar9) * 8 |
              *(uint *)((int)r0_00 - uVar9) >> uVar9 * 8;
      puVar1 = (undefined1 *)((int)&r0_00->pad + 1);
      uVar10 = (uint)puVar1 & 3;
      uVar9 = (uint)&r0_00->vz & 3;
      uVar8 = (*(int *)(puVar1 + -uVar10) << (3 - uVar10) * 8 |
              (uint)(uVar21 >> 0x20) & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar9) * 8 |
              *(uint *)((int)&r0_00->vz - uVar9) >> uVar9 * 8;
      puVar1 = puVar20 + 0x33;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | uVar5 >> (3 - uVar10) * 8;
      puVar1 = puVar20 + 0x30;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & 0xffffffffU >> (4 - uVar10) * 8 | uVar5 << uVar10 * 8;
      puVar1 = puVar20 + 0x37;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | uVar8 >> (3 - uVar10) * 8;
      puVar1 = puVar20 + 0x34;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & 0xffffffffU >> (4 - uVar10) * 8 | uVar8 << uVar10 * 8;
      uVar13 = *(uint *)(puVar20 + 0x60);
      psVar17 = psVar16 + 6;
      uVar10 = uVar13 + 3 & 3;
      uVar9 = uVar13 & 3;
      uVar5 = (*(int *)((uVar13 + 3) - uVar10) << (3 - uVar10) * 8 |
              uVar5 & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar9) * 8 |
              *(uint *)(uVar13 - uVar9) >> uVar9 * 8;
      uVar10 = uVar13 + 7 & 3;
      uVar9 = uVar13 + 4 & 3;
      uVar8 = (*(int *)((uVar13 + 7) - uVar10) << (3 - uVar10) * 8 |
              uVar8 & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar9) * 8 |
              *(uint *)((uVar13 + 4) - uVar9) >> uVar9 * 8;
      puVar1 = puVar20 + 0x3b;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | uVar5 >> (3 - uVar10) * 8;
      puVar1 = puVar20 + 0x38;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & 0xffffffffU >> (4 - uVar10) * 8 | uVar5 << uVar10 * 8;
      puVar1 = puVar20 + 0x3f;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | uVar8 >> (3 - uVar10) * 8;
      puVar1 = puVar20 + 0x3c;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & 0xffffffffU >> (4 - uVar10) * 8 | uVar8 << uVar10 * 8;
      uVar13 = *(uint *)(puVar20 + 0x68);
      iVar19 = iVar19 + -1;
      uVar10 = uVar13 + 3 & 3;
      uVar9 = uVar13 & 3;
      uVar5 = (*(int *)((uVar13 + 3) - uVar10) << (3 - uVar10) * 8 |
              uVar5 & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar9) * 8 |
              *(uint *)(uVar13 - uVar9) >> uVar9 * 8;
      uVar10 = uVar13 + 7 & 3;
      uVar9 = uVar13 + 4 & 3;
      uVar9 = (*(int *)((uVar13 + 7) - uVar10) << (3 - uVar10) * 8 |
              uVar8 & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar9) * 8 |
              *(uint *)((uVar13 + 4) - uVar9) >> uVar9 * 8;
      puVar1 = puVar20 + 0x43;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | uVar5 >> (3 - uVar10) * 8;
      puVar1 = puVar20 + 0x40;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & 0xffffffffU >> (4 - uVar10) * 8 | uVar5 << uVar10 * 8;
      puVar1 = puVar20 + 0x47;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & -1 << (uVar10 + 1) * 8 | uVar9 >> (3 - uVar10) * 8;
      puVar1 = puVar20 + 0x44;
      uVar10 = (uint)puVar1 & 3;
      *(uint *)(puVar1 + -uVar10) =
           *(uint *)(puVar1 + -uVar10) & 0xffffffffU >> (4 - uVar10) * 8 | uVar9 << uVar10 * 8;
      sVar2 = psVar16[10];
      iVar4 = iVar18 + psVar16[8] * 8;
      *(int *)(puVar20 + 0x60) = iVar4;
      *(int *)(puVar20 + 0x68) = iVar18 + sVar2 * 8;
      gte_stsxy3_gt3(r0);
      gte_nclip();
      r0_00 = (SVECTOR *)(iVar18 + *psVar17 * 8);
      gte_stopz((long *)(puVar20 + 0x48));
      uVar21 = CONCAT44(iVar4,*(int *)(puVar20 + 0x48));
      if (-1 < *(int *)(puVar20 + 0x48)) {
        gte_avsz3();
        puVar14[-6] = puVar14[-6] | 0x34000000;
        puVar14[-6] = *(uint *)(param_1 + 100);
        puVar14[-3] = *(uint *)(param_1 + 100);
        *puVar14 = *(uint *)(param_1 + 100);
        gte_stotz((long *)(puVar20 + 0x4c));
        uVar10 = *(ushort *)(*(int *)(puVar20 + 0x50) + 0x8c) & 0xc0;
        if (uVar10 == 0x80) {
LAB_80016df0:
          uVar10 = (uint)*pbVar12 * 0x1000;
          puVar15 = &DAT_800aa6d8 + uVar10;
          uVar9 = *(uint *)(puVar20 + 0x4c) >> 4;
LAB_80016e2c:
          unaff_s2 = (uint *)(puVar15 + uVar9 * 4);
        }
        else if (uVar10 < 0x81) {
          if (uVar10 == 0x40) {
            uVar10 = (uint)*pbVar12 * 0x40;
            puVar15 = &DAT_800ac6d8 + uVar10;
            uVar9 = *(uint *)(puVar20 + 0x4c) >> 10;
            goto LAB_80016e2c;
          }
        }
        else if (uVar10 == 0xc0) {
          iVar4 = FUN_8002178c(0);
          if (iVar4 == 0) goto LAB_80016df0;
          uVar10 = (uint)*pbVar12 * 0x20;
          puVar15 = &UNK_800aa698 + uVar10;
          uVar9 = *(uint *)(puVar20 + 0x4c) >> 0xb;
          goto LAB_80016e2c;
        }
        if ((*(ushort *)(*(int *)(puVar20 + 0x50) + 0x8c) & 0x20) == 0) {
          uVar9 = *(uint *)(puVar20 + 0x58);
          *(uint *)(puVar20 + 0x4c) = *unaff_s2;
          *unaff_s2 = (uint)r0 & uVar9;
          uVar9 = uVar9 & *(uint *)(puVar20 + 0x4c);
          uVar21 = CONCAT44(uVar10,uVar9) | 0x9000000;
          *r0 = uVar9 | 0x9000000;
        }
        else {
          *(uint **)(puVar20 + 0x10) = r0 + 6;
          *(uint **)(puVar20 + 0x14) = r0 + 9;
          *(uint **)(puVar20 + 0x18) = r0 + 1;
          *(uint **)(puVar20 + 0x1c) = r0 + 4;
          *(uint **)(puVar20 + 0x20) = puVar14;
          *(uint **)(puVar20 + 0x28) = unaff_s2;
          *(undefined4 **)(puVar20 + 0x2c) = &DAT_800b855c;
          *(undefined4 *)(puVar20 + 0x24) = DAT_800b8558;
          uVar21 = DivideGT3(puVar20 + 0x30,puVar20 + 0x38,puVar20 + 0x40,r0 + 3);
          DAT_800b8558 = (undefined4)uVar21;
        }
      }
      puVar14 = puVar14 + 0x14;
      r0 = r0 + 0x14;
      psVar16 = psVar17;
    } while (iVar19 != 0);
  }
  return;
}
