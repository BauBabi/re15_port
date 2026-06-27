/* FUN_80016c14 @ 0x80016c14  (Ghidra headless, raw MIPS overlay) */

void FUN_80016c14(void)

{
  ushort uVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  uint *puVar7;
  short sVar8;
  short sVar9;
  int *piVar10;
  int iVar11;
  short sVar12;
  short sVar13;
  MATRIX local_c8;
  MATRIX local_a8;
  SVECTOR local_88;
  SVECTOR local_80;
  SVECTOR local_78;
  SVECTOR local_70;
  int local_68;
  int local_64;
  int local_60;
  short local_5c [2];
  long alStack_58 [2];
  undefined *local_50;
  int local_48;
  uint local_40;
  int local_38;
  int local_34;
  int local_30;
  
  local_48 = 5;
  piVar10 = (int *)&DAT_800ce624;
  local_40 = 0xffffff;
  local_50 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  do {
    if ((*piVar10 & 0x4000000000000) != 0) {
      local_c8.m[0][0] = 0x1000;
      local_c8.m[0][1] = 0;
      local_c8.m[0][2] = 0;
      local_c8.m[1][0] = 0;
      local_c8.m[1][1] = 0x1000;
      local_c8.m[1][2] = 0;
      local_c8.m[2][0] = 0;
      local_c8.m[2][1] = 0;
      local_c8.m[2][2] = 0x1000;
      local_c8._18_2_ = 0;
      RotMatrixY((int)(short)piVar10[-2],&local_c8);
      local_c8.t[0] = (long)(short)piVar10[-4];
      local_c8.t[1] = (long)*(short *)((int)piVar10 + -0xe);
      local_c8.t[2] = (long)(short)piVar10[-3];
      FUN_8002ce94(&DAT_800dcba8,&local_c8,&local_a8);
      SetRotMatrix(&local_a8);
      SetTransMatrix(&local_a8);
      uVar1 = *(ushort *)(piVar10 + -5);
      local_88._0_4_ = (ushort)~uVar1 - 1;
      sVar2 = local_88.vx;
      local_80._0_4_ = SEXT24((short)uVar1);
      local_88.vz = *(short *)((int)piVar10 + -0x12);
      local_34 = (int)local_88.vz;
      local_80.vz = *(short *)((int)piVar10 + -0x12);
      local_78.vz = ~*(ushort *)((int)piVar10 + -0x12) - 1;
      local_30 = (int)local_78.vz;
      local_68 = local_c8.t[0];
      local_64 = local_c8.t[1];
      local_60 = local_c8.t[2];
      local_5c[0] = local_a8.m[0][0];
      local_5c[1] = local_a8.m[0][1];
      sVar12 = (short)(((int)(short)uVar1 - (int)local_88.vx) / 3);
      sVar9 = sVar12 + local_88.vx;
      local_38 = *piVar10;
      sVar8 = sVar12 * 2 + local_88.vx;
      sVar12 = (short)((local_30 - local_34) / 3);
      sVar13 = sVar12 + local_88.vz;
      sVar12 = sVar12 * 2 + local_88.vz;
      local_78._0_4_ = local_88._0_4_;
      local_70._0_4_ = local_80._0_4_;
      local_70.vz = local_78.vz;
      iVar3 = FUN_8002c820(&local_68,DAT_800ce338);
      if (iVar3 != 0) {
        iVar11 = 9;
        iVar3 = local_38;
        do {
          switch(iVar11) {
          case 1:
          case 4:
          case 7:
            local_88.vx = sVar8;
            local_80.vx = uVar1;
            local_70.vx = uVar1;
            local_78.vx = sVar8;
            break;
          case 2:
          case 5:
          case 8:
            local_88.vx = sVar9;
            local_80.vx = sVar8;
            local_70.vx = sVar8;
            local_78.vx = sVar9;
            break;
          case 3:
            local_88._0_4_ = CONCAT22(local_88.vy,sVar2);
            local_80.vx = sVar9;
            local_70.vx = sVar9;
            local_78.vz = (short)local_30;
            local_78.vx = sVar2;
            local_88.vz = sVar12;
            local_80.vz = sVar12;
            local_70.vz = local_78.vz;
            break;
          case 6:
            local_88._0_4_ = CONCAT22(local_88.vy,sVar2);
            local_80.vx = sVar9;
            local_70.vx = sVar9;
            local_78.vx = sVar2;
            local_88.vz = sVar13;
            local_80.vz = sVar13;
            local_78.vz = sVar12;
            local_70.vz = sVar12;
            break;
          case 9:
            local_88._0_4_ = CONCAT22(local_88.vy,sVar2);
            local_80.vx = sVar9;
            local_70.vx = sVar9;
            local_78.vx = sVar2;
            local_88.vz = (short)local_34;
            local_80.vz = (short)local_34;
            local_78.vz = sVar13;
            local_70.vz = sVar13;
          }
          puVar7 = (uint *)(iVar3 + (uint)DAT_800ce5e0 * 0x28 + 0x18);
          local_38 = iVar3;
          uVar4 = RotAverage4(&local_88,&local_80,&local_78,&local_70,(long *)(puVar7 + 2),
                              (long *)(puVar7 + 4),(long *)(puVar7 + 6),(long *)(puVar7 + 8),
                              alStack_58,alStack_58);
          if (0x1f < uVar4) {
            uVar4 = uVar4 - 0x20;
          }
          uVar5 = uVar4 >> 3;
          if (0x1fff < uVar4) {
            uVar5 = 0x3ff;
          }
          puVar6 = (uint *)(local_50 + uVar5 * 4);
          iVar3 = local_38 + 0x68;
          *puVar7 = *puVar7 & 0xff000000 | *puVar6 & local_40;
          iVar11 = iVar11 + -1;
          *puVar6 = *puVar6 & 0xff000000 | (uint)puVar7 & local_40;
        } while (iVar11 != 0);
      }
    }
    piVar10 = piVar10 + 7;
    local_48 = local_48 + -1;
  } while (local_48 != 0);
  return;
}


