/* FUN_80029614 @ 0x80029614  (Ghidra headless, raw MIPS overlay) */

bool FUN_80029614(int param_1,int param_2,uint *param_3,uint param_4)

{
  bool bVar1;
  short sVar2;
  byte bVar3;
  uint uVar4;
  VECTOR *r0;
  int *piVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  uint uVar9;
  SVECTOR *r0_00;
  uint uVar10;
  uint *puVar11;
  uint *puVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  undefined1 *puVar17;
  int iVar18;
  undefined1 auStack_78 [16];
  int local_68;
  int local_64;
  int local_60;
  
  iVar15 = DAT_800ce330;
  puVar17 = auStack_78;
  piVar5 = (int *)(param_1 + (uint)*(ushort *)(param_1 + 2) +
                  (*(uint *)(param_1 + 4) >> 0x12) * (*param_3 & 0xfff) * 4);
  iVar18 = (*piVar5 >> 0x10) * (int)*(short *)(DAT_800ce330 + 0x1de);
  puVar11 = *(uint **)(DAT_800ce330 + 0x198);
  uVar16 = (uint)*(ushort *)((uint)*(byte *)(DAT_800ce330 + 0x14c) * 4 + param_2);
  puVar11[0xb] = (int)(short)*piVar5;
  puVar11[0xc] = iVar18 >> 0xc;
  puVar11[0xd] = (int)(short)piVar5[1];
  uVar13 = (uint)*(ushort *)(param_1 + 4);
  uVar10 = (uint)*(byte *)(iVar15 + 0x14e);
  puVar12 = puVar11;
  uVar14 = uVar13;
  if (uVar10 != 0) {
    iVar18 = (param_4 & 0xffff) * uVar10;
    local_68 = (int)(short)puVar11[0xe];
    local_64 = (int)*(short *)((int)puVar11 + 0x3a);
    local_60 = (int)(short)puVar11[0xf];
    gte_ldIR0(0x1000 - iVar18);
    r0 = (VECTOR *)(puVar11 + 0xb);
    gte_ldlvl(r0);
    gte_gpf12();
    gte_ldIR0(iVar18);
    gte_ldlvl((VECTOR *)(puVar17 + 0x10));
    gte_gpl12();
    gte_stlvl(r0);
  }
  puVar6 = (uint *)(piVar5 + 3);
  *(short *)(puVar12 + 0xe) = (short)puVar12[0xb];
  *(short *)((int)puVar12 + 0x3a) = (short)puVar12[0xc];
  *(short *)(puVar12 + 0xf) = (short)puVar12[0xd];
  do {
    puVar12[0x1f] = *puVar6 & 0xfff | (*puVar6 & 0xfff000) << 4;
    puVar7 = puVar6 + 1;
    *(ushort *)(puVar12 + 0x20) =
         (ushort)*(byte *)((int)puVar6 + 3) | (ushort)((*puVar7 & 0xf) << 8);
    if (uVar14 == 1) break;
    puVar12[0x4a] = (*puVar7 & 0xfff0) >> 4 | *puVar7 & 0xfff0000;
    *(ushort *)(puVar12 + 0x4b) = (ushort)(*puVar7 >> 0x1c) | (ushort)(byte)puVar6[2] << 4;
    if (uVar14 == 2) break;
    uVar4 = puVar6[2];
    puVar12[0x75] = (uVar4 & 0xfff00) >> 8 | (uVar4 & 0xfff00000) >> 4;
    *(ushort *)(puVar12 + 0x76) = (ushort)puVar6[3] & 0xfff;
    if (uVar14 == 3) break;
    uVar4 = puVar6[3];
    puVar7 = puVar6 + 4;
    puVar12[0xa0] = (uVar4 & 0xfff000) >> 0xc | (uVar4 & 0xff000000) >> 8 | (*puVar7 & 0xf) << 0x18;
    *(short *)(puVar12 + 0xa1) = (short)((*puVar7 & 0xfff0) >> 4);
    if (uVar14 == 4) break;
    puVar8 = puVar6 + 5;
    puVar12[0xcb] =
         (int)(*puVar7 & 0xfff0000) >> 0x10 |
         (*puVar7 & 0xf0000000) >> 0xc | (uint)(byte)*puVar8 << 0x14;
    *(ushort *)(puVar12 + 0xcc) = (ushort)(*puVar8 >> 8) & 0xfff;
    if (uVar14 == 5) break;
    puVar12[0xf6] = *puVar8 >> 0x14 | (puVar6[6] & 0xfff) << 0x10;
    *(short *)(puVar12 + 0xf7) = (short)((puVar6[6] & 0xfff000) >> 0xc);
    if (uVar14 == 6) break;
    puVar7 = puVar6 + 7;
    puVar12[0x121] =
         (uint)*(byte *)((int)puVar6 + 0x1b) | (*puVar7 & 0xf) << 8 | (*puVar7 & 0xfff0) << 0xc;
    *(ushort *)(puVar12 + 0x122) = (ushort)(*puVar7 >> 0x10) & 0xfff;
    if (uVar14 == 7) break;
    puVar8 = puVar6 + 8;
    uVar14 = uVar14 - 8;
    uVar4 = *puVar8;
    puVar12[0x14c] = *puVar7 >> 0x1c | (uVar4 & 0xff) << 4 | (uVar4 & 0xfff00) << 8;
    puVar6 = puVar6 + 9;
    *(ushort *)(puVar12 + 0x14d) = (ushort)(*puVar8 >> 0x14);
    puVar12 = puVar12 + 0x158;
  } while (uVar14 != 0);
  puVar12 = puVar11;
  uVar14 = uVar13;
  if (uVar10 == 0) {
    do {
      uVar13 = uVar13 - 1;
      puVar11[0x1a] = puVar11[0x1f];
      *(short *)(puVar11 + 0x1b) = (short)puVar11[0x20];
      puVar11 = puVar11 + 0x2b;
    } while (uVar13 != 0);
  }
  else {
    *(char *)(iVar15 + 0x14e) = *(char *)(iVar15 + 0x14e) + -1;
    do {
      uVar4 = (ushort)puVar11[0x1b] & 0xfff;
      puVar11[0x1a] = puVar11[0x1a] & 0xfff0fff;
      *(short *)(puVar11 + 0x1b) = (short)uVar4;
      gte_ldIR0(iVar18);
      gte_ldsv((SVECTOR *)(puVar11 + 0x1a));
      uVar9 = ((int)(short)puVar11[0x1f] - (int)(short)puVar11[0x1a]) + 0x800;
      uVar10 = ((int)*(short *)((int)puVar11 + 0x7e) - (int)*(short *)((int)puVar11 + 0x6a)) + 0x800
      ;
      uVar4 = ((int)(short)puVar11[0x20] - uVar4) + 0x800;
      gte_gpf12_b();
      if (0x1000 < uVar9) {
        sVar2 = 0x1000;
        if ((uVar9 & 0x8000) == 0) {
          sVar2 = -0x1000;
        }
        *(short *)(puVar11 + 0x1f) = (short)puVar11[0x1f] + sVar2;
      }
      if (0x1000 < uVar10) {
        sVar2 = 0x1000;
        if ((uVar10 & 0x8000) == 0) {
          sVar2 = -0x1000;
        }
        *(short *)((int)puVar11 + 0x7e) = *(short *)((int)puVar11 + 0x7e) + sVar2;
      }
      if (0x1000 < uVar4) {
        sVar2 = 0x1000;
        if ((uVar4 & 0x8000) == 0) {
          sVar2 = -0x1000;
        }
        *(short *)(puVar11 + 0x20) = (short)puVar11[0x20] + sVar2;
      }
      gte_ldIR0(0x1000 - iVar18);
      gte_ldsv((SVECTOR *)(puVar11 + 0x1f));
      r0_00 = (SVECTOR *)(puVar11 + 0x1a);
      uVar13 = uVar13 - 1;
      gte_gpl12_b();
      puVar11 = puVar11 + 0x2b;
      gte_stsv(r0_00);
    } while (uVar13 != 0);
  }
  do {
    if ((*puVar12 & 0x200) == 0) {
      RotMatrix((SVECTOR *)(puVar12 + 0x1a),(MATRIX *)(puVar12 + 6));
    }
    uVar14 = uVar14 - 1;
    puVar12 = puVar12 + 0x2b;
  } while (uVar14 != 0);
  bVar3 = *(char *)(iVar15 + 0x14d) + 1;
  *(byte *)(iVar15 + 0x14d) = bVar3;
  bVar1 = uVar16 <= bVar3;
  if (bVar1) {
    *(undefined1 *)(iVar15 + 0x14d) = 0;
  }
  return bVar1;
}


