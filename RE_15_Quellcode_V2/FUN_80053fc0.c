void FUN_80053fc0(int *param_1)

{
  ushort uVar1;
  undefined4 uVar2;
  int iVar3;
  long lVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  undefined4 *puVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int local_38;
  int local_34;
  int local_30;
  undefined4 local_28;
  
  iVar14 = 0;
  iVar12 = 0;
  iVar13 = 0;
  iVar15 = 0;
  do {
    iVar10 = DAT_800b0fe4 * 0x28;
    iVar7 = iVar10 + *(int *)(DAT_800ac778 + 0x2c);
    iVar3 = iVar7 + iVar12;
    if (*(char *)(iVar7 + iVar14 + 1) == '\0') {
      local_38 = *param_1 - (int)*(short *)(iVar3 + 0x10) >> 4;
      local_34 = param_1[1] -
                 (int)*(short *)(iVar10 + *(int *)(DAT_800ac778 + 0x2c) + iVar12 + 0x12) >> 4;
      local_30 = param_1[2] -
                 (int)*(short *)(iVar10 + *(int *)(DAT_800ac778 + 0x2c) + iVar12 + 0x14) >> 4;
      lVar4 = SquareRoot0(local_38 * local_38 + local_30 * local_30);
      uVar2 = local_28;
      iVar3 = DAT_800b0fe4 * 0x28;
      iVar10 = iVar3 + *(int *)(DAT_800ac778 + 0x2c);
      uVar1 = *(ushort *)(iVar15 + iVar10 + 0x22);
      uVar8 = (uint)(uVar1 >> 4);
      iVar7 = uVar8 - lVar4;
      if (iVar7 < 0) {
        iVar7 = 0;
      }
      if (uVar1 == 0) {
        local_28 = 0;
      }
      else {
        iVar10 = (uint)*(byte *)(iVar10 + iVar13 + 4) * iVar7;
        if (uVar8 == 0) {
          trap(0x1c00);
        }
        if ((uVar8 == 0xffffffff) && (iVar10 == -0x80000000)) {
          trap(0x1800);
        }
        iVar9 = iVar3 + *(int *)(DAT_800ac778 + 0x2c);
        iVar16 = (uint)*(byte *)(iVar9 + iVar13 + 5) * iVar7;
        uVar5 = (uint)(*(ushort *)(iVar15 + iVar9 + 0x22) >> 4);
        if (uVar5 == 0) {
          trap(0x1c00);
        }
        if ((uVar5 == 0xffffffff) && (iVar16 == -0x80000000)) {
          trap(0x1800);
        }
        iVar3 = iVar3 + *(int *)(DAT_800ac778 + 0x2c);
        iVar7 = (uint)*(byte *)(iVar3 + iVar13 + 6) * iVar7;
        uVar6 = (uint)(*(ushort *)(iVar15 + iVar3 + 0x22) >> 4);
        if (uVar6 == 0) {
          trap(0x1c00);
        }
        if ((uVar6 == 0xffffffff) && (iVar7 == -0x80000000)) {
          trap(0x1800);
        }
        local_28._3_1_ = SUB41(uVar2,3);
        local_28._0_3_ =
             CONCAT12((char)(iVar7 / (int)uVar6),
                      CONCAT11((char)(iVar16 / (int)uVar5),(char)(iVar10 / (int)uVar8)));
      }
      puVar11 = &local_28;
    }
    else {
      local_38 = (int)*(short *)(iVar3 + 0x10);
      local_34 = (int)*(short *)(iVar10 + *(int *)(DAT_800ac778 + 0x2c) + iVar12 + 0x12);
      local_30 = (int)*(short *)(iVar10 + *(int *)(DAT_800ac778 + 0x2c) + iVar12 + 0x14);
      puVar11 = (undefined4 *)(iVar10 + *(int *)(DAT_800ac778 + 0x2c) + iVar13 + 4);
    }
    iVar12 = iVar12 + 6;
    FUN_800542dc(iVar14,&local_38,puVar11);
    iVar13 = iVar13 + 3;
    iVar14 = iVar14 + 1;
    iVar15 = iVar15 + 2;
  } while (iVar14 < 3);
  iVar12 = DAT_800b0fe4 * 0x28 + *(int *)(DAT_800ac778 + 0x2c);
  SetBackColor((uint)*(byte *)(iVar12 + 0xd),(uint)*(byte *)(iVar12 + 0xe),
               (uint)*(byte *)(iVar12 + 0xf));
  return;
}
