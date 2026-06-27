/* FUN_80076f88 @ 0x80076f88  (Ghidra headless, raw MIPS overlay) */

void FUN_80076f88(int *param_1)

{
  undefined4 uVar1;
  long lVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int local_38;
  int local_34;
  int local_30;
  undefined4 local_28;
  
  iVar10 = 0;
  iVar14 = 4;
  iVar13 = 0;
  iVar11 = *(int *)(DAT_800ce324 + 0x2c) + DAT_800d4820 * 0x28;
  iVar9 = iVar11;
  iVar12 = iVar11;
  do {
    if (*(char *)(iVar11 + iVar10 + 1) == '\0') {
      local_38 = *param_1 - (int)*(short *)(iVar9 + 0x10) >> 4;
      local_34 = param_1[1] - (int)*(short *)(iVar9 + 0x12) >> 4;
      local_30 = param_1[2] - (int)*(short *)(iVar9 + 0x14) >> 4;
      lVar2 = SquareRoot0(local_38 * local_38 + local_30 * local_30);
      uVar1 = local_28;
      uVar5 = (uint)(*(ushort *)(iVar12 + 0x22) >> 4);
      iVar7 = uVar5 - lVar2;
      if (iVar7 < 0) {
        iVar7 = 0;
      }
      iVar6 = iVar11 + iVar13;
      if (*(ushort *)(iVar12 + 0x22) == 0) {
        local_28 = 0;
      }
      else {
        iVar15 = (uint)*(byte *)(iVar6 + 4) * iVar7;
        if (uVar5 == 0) {
          trap(0x1c00);
        }
        if ((uVar5 == 0xffffffff) && (iVar15 == -0x80000000)) {
          trap(0x1800);
        }
        iVar16 = (uint)*(byte *)(iVar6 + 5) * iVar7;
        uVar3 = (uint)(*(ushort *)(iVar12 + 0x22) >> 4);
        if (uVar3 == 0) {
          trap(0x1c00);
        }
        if ((uVar3 == 0xffffffff) && (iVar16 == -0x80000000)) {
          trap(0x1800);
        }
        iVar7 = (uint)*(byte *)(iVar6 + 6) * iVar7;
        uVar4 = (uint)(*(ushort *)(iVar12 + 0x22) >> 4);
        if (uVar4 == 0) {
          trap(0x1c00);
        }
        if ((uVar4 == 0xffffffff) && (iVar7 == -0x80000000)) {
          trap(0x1800);
        }
        local_28._3_1_ = SUB41(uVar1,3);
        local_28._0_3_ =
             CONCAT12((char)(iVar7 / (int)uVar4),
                      CONCAT11((char)(iVar16 / (int)uVar3),(char)(iVar15 / (int)uVar5)));
      }
      puVar8 = &local_28;
    }
    else {
      local_38 = (int)*(short *)(iVar9 + 0x10);
      local_34 = (int)*(short *)(iVar9 + 0x12);
      local_30 = (int)*(short *)(iVar9 + 0x14);
      puVar8 = (undefined4 *)(iVar11 + iVar14);
    }
    iVar14 = iVar14 + 3;
    FUN_80077200(iVar10,&local_38,puVar8);
    iVar9 = iVar9 + 6;
    iVar13 = iVar13 + 3;
    iVar10 = iVar10 + 1;
    iVar12 = iVar12 + 2;
  } while (iVar10 < 3);
  SetBackColor((uint)*(byte *)(iVar11 + 0xd),(uint)*(byte *)(iVar11 + 0xe),
               (uint)*(byte *)(iVar11 + 0xf));
  return;
}


