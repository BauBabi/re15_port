/* FUN_8004ab60 @ 0x8004ab60  (Ghidra headless, raw MIPS overlay) */

void FUN_8004ab60(uint param_1,byte param_2,short param_3,short param_4)

{
  uint uVar1;
  undefined1 uVar2;
  long lVar3;
  long lVar4;
  int iVar5;
  ushort uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  short *psVar13;
  ushort *puVar14;
  ushort *puVar15;
  int iVar16;
  undefined2 *puVar17;
  short sVar18;
  short sVar19;
  short *psVar20;
  short *psVar21;
  undefined2 *puVar22;
  undefined2 *puVar23;
  ushort *puVar24;
  uint uVar25;
  ushort *puVar26;
  ushort *puVar27;
  undefined1 *puVar28;
  int iVar29;
  ushort local_238 [16];
  ushort local_218 [16];
  ushort local_1f8 [16];
  short local_1d8;
  short local_1d6;
  short local_1d4;
  short local_1d2;
  short local_1d0;
  short local_1ce;
  byte local_190 [16];
  VECTOR local_180;
  VECTOR local_170;
  VECTOR local_160;
  undefined1 local_150;
  char local_14f;
  undefined2 local_140 [3];
  undefined2 local_13a;
  short local_138;
  short local_136;
  short local_134;
  short local_132;
  byte local_c0;
  short local_b8;
  short local_b0;
  ushort local_a8;
  int local_a0;
  char local_98;
  int local_90;
  int local_88;
  int local_80;
  int local_78;
  int local_70;
  short *local_38;
  ushort *local_34;
  int local_30;
  undefined2 *local_2c;
  
  local_c0 = param_2;
  puVar24 = local_238;
  psVar13 = &local_1d8;
  puVar17 = local_140;
  puVar28 = &local_150;
  uVar25 = 0;
  local_b8 = param_3;
  local_b0 = param_4;
  local_a0 = 0x7fffffff;
  local_a8 = (ushort)(1 << (param_2 & 0x1f));
  iVar16 = *(int *)(DAT_800ce324 + 0x38);
  local_70 = DAT_800ce330;
  local_238[0] = *(ushort *)((param_1 & 0xff) * 0xc + iVar16 + 0xe);
  local_218[0] = (ushort)(1 << (param_1 & 0x1f));
  local_1f8[0] = 0;
  local_190[0] = (byte)param_1;
  local_1d8 = (short)*(undefined4 *)(DAT_800ce330 + 0x38);
  local_1d6 = (short)*(undefined4 *)(DAT_800ce330 + 0x40);
  if (local_238[0] != 0) {
    puVar14 = local_218;
    psVar20 = &local_1d6;
    puVar22 = &local_13a;
    puVar26 = local_1f8;
    do {
      uVar6 = *puVar24;
      if (uVar6 == 0) {
        puVar24 = puVar24 + -1;
        uVar25 = uVar25 - 1;
        param_1 = (uint)local_190[uVar25 & 0xff];
        puVar28 = puVar28 + -1;
        psVar13 = psVar13 + -2;
        puVar15 = puVar14 + -1;
        puVar17 = puVar17 + -4;
        psVar21 = psVar20 + -2;
        puVar23 = puVar22 + -4;
        puVar27 = puVar26 + -1;
      }
      else if ((uVar6 & local_a8) == 0) {
        uVar9 = param_1 & 0xff;
        param_1 = 0;
        if ((uVar6 & 1) == 0) {
          uVar1 = 1;
          do {
            param_1 = uVar1;
            uVar1 = param_1 + 1;
          } while (((int)(uint)uVar6 >> (param_1 & 0x1f) & 1U) == 0);
        }
        puVar27 = puVar26 + 1;
        psVar21 = psVar20 + 2;
        psVar13 = psVar13 + 2;
        puVar23 = puVar22 + 4;
        puVar17 = puVar17 + 4;
        puVar28 = puVar28 + 1;
        iVar10 = iVar16 + uVar9 * 0xc;
        iVar11 = iVar16 + (param_1 & 0xff) * 0xc;
        *puVar24 = *puVar24 & ~(ushort)(1 << (param_1 & 0x1f));
        iVar29 = (int)*(short *)(iVar10 + 10);
        iVar8 = (int)*(short *)(iVar11 + 6);
        iVar5 = (int)*(short *)(iVar10 + 6);
        iVar12 = (int)*(short *)(iVar11 + 10);
        if ((iVar5 == iVar12) || (iVar29 == iVar8)) {
          sVar18 = *(short *)(iVar10 + 6);
          if (iVar29 == iVar8) {
            sVar18 = *(short *)(iVar10 + 10);
          }
          iVar29 = (int)sVar18;
          iVar8 = (int)*(short *)(iVar10 + 4);
          if ((int)*(short *)(iVar10 + 4) < (int)*(short *)(iVar11 + 4)) {
            iVar8 = (int)*(short *)(iVar11 + 4);
          }
          iVar7 = (int)*(short *)(iVar10 + 8);
          if ((int)*(short *)(iVar11 + 8) < (int)*(short *)(iVar10 + 8)) {
            iVar7 = (int)*(short *)(iVar11 + 8);
          }
          sVar19 = (short)((uint)(iVar8 + iVar7) >> 1);
          uVar2 = 0;
          iVar5 = iVar29;
        }
        else {
          sVar19 = *(short *)(iVar10 + 4);
          if (*(short *)(iVar10 + 8) == *(short *)(iVar11 + 4)) {
            sVar19 = *(short *)(iVar11 + 4);
          }
          iVar7 = (int)sVar19;
          if (iVar5 < iVar8) {
            iVar5 = iVar8;
          }
          if (iVar12 < iVar29) {
            iVar29 = iVar12;
          }
          sVar18 = (short)((uint)(iVar5 + iVar29) >> 1);
          uVar2 = 1;
          iVar8 = iVar7;
        }
        uVar25 = uVar25 + 1;
        puVar24 = puVar24 + 1;
        puVar15 = puVar14 + 1;
        *puVar17 = (short)iVar8;
        puVar22[2] = (short)iVar5;
        puVar22[3] = (short)iVar7;
        *puVar23 = (short)iVar29;
        *puVar28 = uVar2;
        iVar29 = (int)psVar20[-1] - (int)sVar19;
        if (iVar29 < 0) {
          iVar29 = -iVar29;
        }
        iVar5 = (int)*psVar20 - (int)sVar18;
        if (iVar5 < 0) {
          iVar5 = -iVar5;
        }
        *puVar27 = *puVar26 + (short)iVar29 + (short)iVar5;
        *psVar13 = sVar19;
        *psVar21 = sVar18;
        local_190[uVar25 & 0xff] = (byte)param_1;
        uVar6 = *puVar14 | (ushort)(1 << (param_1 & 0x1f));
        *puVar15 = uVar6;
        *puVar24 = *(ushort *)((param_1 & 0xff) * 0xc + iVar16 + 0xe) & ~uVar6;
      }
      else {
        *puVar24 = uVar6 & ~local_a8;
        iVar11 = iVar16 + (param_1 & 0xff) * 0xc;
        iVar29 = (int)*(short *)(iVar11 + 10);
        iVar5 = (int)*(short *)(iVar11 + 6);
        iVar10 = iVar16 + (uint)local_c0 * 0xc;
        iVar8 = (int)*(short *)(iVar10 + 6);
        iVar12 = (int)*(short *)(iVar10 + 10);
        if ((iVar5 == iVar12) || (iVar29 == iVar8)) {
          sVar18 = *(short *)(iVar11 + 6);
          if (iVar29 == iVar8) {
            sVar18 = *(short *)(iVar11 + 10);
          }
          iVar29 = (int)sVar18;
          iVar8 = (int)*(short *)(iVar11 + 4);
          if ((int)*(short *)(iVar11 + 4) < (int)*(short *)(iVar10 + 4)) {
            iVar8 = (int)*(short *)(iVar10 + 4);
          }
          iVar7 = (int)*(short *)(iVar11 + 8);
          if ((int)*(short *)(iVar10 + 8) < (int)*(short *)(iVar11 + 8)) {
            iVar7 = (int)*(short *)(iVar10 + 8);
          }
          sVar19 = (short)((uint)(iVar8 + iVar7) >> 1);
          uVar2 = 0;
          iVar5 = iVar29;
        }
        else {
          sVar19 = *(short *)(iVar11 + 4);
          if (*(short *)(iVar11 + 8) == *(short *)(iVar10 + 4)) {
            sVar19 = *(short *)(iVar10 + 4);
          }
          iVar7 = (int)sVar19;
          if (iVar5 < iVar8) {
            iVar5 = iVar8;
          }
          if (iVar12 < iVar29) {
            iVar29 = iVar12;
          }
          sVar18 = (short)((uint)(iVar5 + iVar29) >> 1);
          uVar2 = 1;
          iVar8 = iVar7;
        }
        puVar22[1] = (short)iVar8;
        puVar22[2] = (short)iVar5;
        puVar22[3] = (short)iVar7;
        puVar22[4] = (short)iVar29;
        puVar28[1] = uVar2;
        psVar20[1] = sVar19;
        psVar20[2] = sVar18;
        psVar20[3] = local_b8;
        psVar20[4] = local_b0;
        iVar29 = (int)*psVar13 - (int)sVar19 >> 3;
        iVar5 = (int)*psVar20 - (int)sVar18 >> 3;
        local_38 = psVar13;
        local_34 = puVar14;
        local_30 = iVar16;
        local_2c = puVar17;
        lVar3 = SquareRoot0(iVar29 * iVar29 + iVar5 * iVar5);
        iVar16 = (int)local_b8 - (int)sVar19 >> 3;
        iVar29 = (int)local_b0 - (int)sVar18 >> 3;
        lVar4 = SquareRoot0(iVar16 * iVar16 + iVar29 * iVar29);
        iVar29 = (uint)*puVar26 + lVar3 + lVar4;
        local_190[(uVar25 & 0xff) + 1] = local_c0;
        psVar13 = local_38;
        puVar15 = local_34;
        iVar16 = local_30;
        puVar17 = local_2c;
        psVar21 = psVar20;
        puVar23 = puVar22;
        puVar27 = puVar26;
        if (iVar29 < local_a0) {
          param_1 = (uint)local_190[1];
          local_90 = (int)local_1d4;
          local_88 = (int)local_1d2;
          local_98 = local_14f;
          local_a0 = iVar29;
          local_170.vx = (int)local_138;
          local_170.vz = (int)local_136;
          local_160.vx = (int)local_134;
          local_160.vz = (int)local_132;
          local_80 = (int)local_1d0;
          local_78 = (int)local_1ce;
        }
      }
      puVar14 = puVar15;
      psVar20 = psVar21;
      puVar22 = puVar23;
      puVar26 = puVar27;
    } while ((local_238[0] != 0) || (puVar24 != local_238));
  }
  if (local_98 == '\0') {
    iVar16 = local_88 - *(int *)(local_70 + 0x40);
    if (iVar16 == 0) {
      iVar29 = 0x7fffffff;
    }
    else {
      iVar5 = (local_90 - *(int *)(local_70 + 0x38)) * 0x1000;
      iVar29 = iVar5 / iVar16;
      if (iVar16 == 0) {
        trap(0x1c00);
      }
      if ((iVar16 == -1) && (iVar5 == -0x80000000)) {
        trap(0x1800);
      }
    }
    if (iVar29 < 0) {
      iVar29 = -iVar29;
    }
    iVar5 = local_88;
    if ((10000 < iVar29) && (iVar5 = local_88 + -800, iVar16 < 0)) {
      iVar5 = local_88 + 800;
    }
    local_88 = iVar5;
    iVar16 = 800;
    iVar29 = 0;
  }
  else {
    iVar16 = local_90 - *(int *)(local_70 + 0x38);
    if (iVar16 == 0) {
      iVar29 = 0x7fffffff;
    }
    else {
      iVar5 = (local_88 - *(int *)(local_70 + 0x40)) * 0x1000;
      iVar29 = iVar5 / iVar16;
      if (iVar16 == 0) {
        trap(0x1c00);
      }
      if ((iVar16 == -1) && (iVar5 == -0x80000000)) {
        trap(0x1800);
      }
    }
    if (iVar29 < 0) {
      iVar29 = -iVar29;
    }
    iVar5 = local_90;
    if ((10000 < iVar29) && (iVar5 = local_90 + -800, iVar16 < 0)) {
      iVar5 = local_90 + 800;
    }
    local_90 = iVar5;
    iVar16 = 0;
    iVar29 = 800;
  }
  local_180.vy = 0;
  local_180.vx = local_80 - *(int *)(local_70 + 0x38);
  local_180.vz = local_78 - *(int *)(local_70 + 0x40);
  local_170.vy = 0;
  local_170.vx = local_170.vx - (*(int *)(local_70 + 0x38) - iVar16);
  local_170.vz = local_170.vz - (*(int *)(local_70 + 0x40) - iVar29);
  local_160.vy = 0;
  local_160.vx = local_160.vx - (*(int *)(local_70 + 0x38) + iVar16);
  local_160.vz = local_160.vz - (*(int *)(local_70 + 0x40) + iVar29);
  OuterProduct0(&local_170,&local_180,&local_170);
  OuterProduct0(&local_160,&local_180,&local_160);
  if ((local_170.vy & 0x80000000U) != (local_160.vy & 0x80000000U)) {
    local_90 = local_80;
    local_88 = local_78;
  }
  *(undefined2 *)(local_70 + 0x1c4) = (undefined2)local_90;
  *(undefined2 *)(local_70 + 0x1c6) = (undefined2)local_88;
  return;
}


