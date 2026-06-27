/* FUN_80040350 @ 0x80040350  (Ghidra headless, raw MIPS overlay) */

void FUN_80040350(int param_1,int param_2)

{
  short sVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  SVECTOR local_a0;
  MATRIX local_98;
  SVECTOR local_78;
  VECTOR local_70;
  VECTOR local_60;
  VECTOR local_50;
  VECTOR VStack_40;
  
  if ((DAT_800cfbf3 < 3) &&
     (iVar10 = (byte)(&DAT_800a4100)[*(ushort *)(param_1 + 0x10e) & 0xfff] - 1,
     (byte)(&DAT_800a4100)[*(ushort *)(param_1 + 0x10e) & 0xfff] != 0)) {
    iVar8 = iVar10 * 8;
    sVar3 = *(short *)(&DAT_800a4114 + iVar8);
    sVar1 = *(short *)(&DAT_800a411a + iVar8);
    iVar9 = *(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20;
    iVar4 = *(int *)(iVar9 + 4);
    iVar5 = *(int *)(iVar9 + 0xc);
    iVar6 = *(int *)(iVar9 + 0x10);
    iVar7 = *(int *)(iVar9 + 0x18);
    sVar2 = FUN_800154ac();
    local_a0.vy = 0;
    local_a0.vz = 0;
    local_98.m[0][0] = 0x1000;
    local_98.m[0][1] = 0;
    local_98.m[0][2] = 0;
    local_98.m[1][0] = 0;
    local_98.m[1][1] = 0x1000;
    local_98.m[1][2] = 0;
    local_98.m[2][0] = 0;
    local_98.m[2][1] = 0;
    local_98.m[2][2] = 0x1000;
    local_98._18_2_ = 0;
    local_a0.vx = sVar3;
    RotMatrixY(sVar2 + 0x400,&local_98);
    ApplyMatrixSV(&local_98,&local_a0,&local_78);
    local_60.vx = (*(int *)(iVar9 + 4) + (int)local_78.vx) - *(int *)(param_2 + 0x5c) >> 4;
    local_60.vy = ((*(int *)(iVar9 + 8) + (int)sVar3) - *(int *)(param_2 + 0x60)) - (int)sVar1 >> 4;
    local_60.vz = (*(int *)(iVar9 + 0xc) + (int)local_78.vz) - *(int *)(param_2 + 100) >> 4;
    local_50.vx = (*(int *)(iVar9 + 4) - (int)local_78.vx) - *(int *)(param_2 + 0x5c) >> 4;
    local_50.vy = ((*(int *)(iVar9 + 8) - (int)sVar3) - *(int *)(param_2 + 0x60)) - (int)sVar1 >> 4;
    local_50.vz = (*(int *)(iVar9 + 0xc) - (int)local_78.vz) - *(int *)(param_2 + 100) >> 4;
    local_78.vx = *(short *)(&DAT_800a4116 + iVar8);
    local_78.vy = *(short *)(&DAT_800a4118 + iVar8);
    local_78.vz = 0;
    ApplyMatrix((MATRIX *)(param_2 + 0x48),&local_78,&VStack_40);
    OuterProduct0(&VStack_40,&local_60,&local_60);
    OuterProduct0(&VStack_40,&local_50,&local_50);
    if ((local_60.vy < 0) &&
       ((((local_50.vy & 0x80000000U) == 0 && (((local_60.vx ^ local_50.vx) & 0x80000000U) != 0)) &&
        (((local_60.vz ^ local_50.vz) & 0x80000000U) != 0)))) {
      local_70.vy = *(int *)(iVar9 + 0x14) - *(int *)(iVar9 + 8);
      local_70.vx = iVar6 - iVar4;
      local_70.vz = iVar7 - iVar5;
      VectorNormal(&local_70,&local_70);
      sVar3 = FUN_80015fe8();
      local_78.vx = ((short)(local_70.vx >> 3) + (short)*(undefined4 *)(iVar9 + 4)) -
                    (sVar3 + -0x80);
      sVar3 = FUN_80015fe8();
      local_78.vy = ((short)(local_70.vy >> 3) + (short)*(undefined4 *)(iVar9 + 8)) -
                    (sVar3 + -0x80);
      sVar3 = FUN_80015fe8();
      local_78.vz = ((short)(local_70.vz >> 3) + (short)*(undefined4 *)(iVar9 + 0xc)) -
                    (sVar3 + -0x80);
      FUN_8001bf10(0x70002c8,0,&DAT_8009db44,&local_78);
      if (iVar10 != 0) {
        local_a0.vx = (short)(local_70.vx >> 3) + (short)*(undefined4 *)(iVar9 + 4);
        local_a0.vy = (short)(local_70.vy >> 3) + (short)*(undefined4 *)(iVar9 + 8);
        local_a0.vz = (short)(local_70.vz >> 3) + (short)*(undefined4 *)(iVar9 + 0xc);
        sVar3 = FUN_80015fe8();
        local_78.vx = local_a0.vx - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vy = local_a0.vy - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vz = local_a0.vz - (sVar3 * 2 + -0x100);
        FUN_8001bf10(0x70002c8,0,&DAT_8009db44,&local_78);
        sVar3 = FUN_80015fe8();
        local_78.vx = local_a0.vx - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vy = local_a0.vy - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vz = local_a0.vz - (sVar3 * 2 + -0x100);
        FUN_8001bf10(0x70002c8,0,&DAT_8009db44,&local_78);
        sVar3 = FUN_80015fe8();
        local_78.vx = local_a0.vx - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vy = local_a0.vy - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vz = local_a0.vz - (sVar3 * 2 + -0x100);
        FUN_8001bf10(0x70002c8,0,&DAT_8009db44,&local_78);
        sVar3 = FUN_80015fe8();
        local_78.vx = local_a0.vx - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vy = local_a0.vy - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vz = local_a0.vz - (sVar3 * 2 + -0x100);
        FUN_8001bf10(0x70002c8,0,&DAT_8009db44,&local_78);
        sVar3 = FUN_80015fe8();
        local_78.vx = local_a0.vx - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vy = local_a0.vy - (sVar3 * 2 + -0x100);
        sVar3 = FUN_80015fe8();
        local_78.vz = local_a0.vz - (sVar3 * 2 + -0x100);
        FUN_8001bf10(0x70002c8,0,&DAT_8009db44,&local_78);
      }
    }
  }
  return;
}


