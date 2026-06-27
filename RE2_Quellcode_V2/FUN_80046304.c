/* FUN_80046304 @ 0x80046304  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80046304(int param_1)

{
  undefined1 *puVar1;
  byte bVar2;
  ushort uVar3;
  short sVar4;
  long lVar5;
  int iVar6;
  uint uVar7;
  undefined4 uVar8;
  int iVar9;
  ushort *puVar10;
  uint *puVar11;
  MATRIX *m;
  uint *puVar12;
  SVECTOR local_a0;
  SVECTOR local_98;
  MATRIX MStack_90;
  short local_70;
  short local_6e;
  short local_6c;
  SVECTOR SStack_60;
  SVECTOR SStack_58;
  undefined1 auStack_50 [4];
  undefined1 auStack_4c [4];
  ushort local_48 [4];
  int local_40;
  char local_38;
  SVECTOR *local_30;
  
  local_40 = 6;
  puVar1 = auStack_50 + 3;
  uVar7 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar7) =
       *(uint *)(puVar1 + -uVar7) & -1 << (uVar7 + 1) * 8 | DAT_8001103c >> (3 - uVar7) * 8;
  auStack_50 = (undefined1  [4])DAT_8001103c;
  puVar1 = auStack_4c + 3;
  uVar7 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar7) =
       *(uint *)(puVar1 + -uVar7) & -1 << (uVar7 + 1) * 8 | DAT_80011040 >> (3 - uVar7) * 8;
  auStack_4c = (undefined1  [4])DAT_80011040;
  local_38 = '\x01';
  puVar11 = (uint *)(*(int *)(param_1 + 0x198) + 0xd70);
  if (*(char *)(param_1 + 8) == '\x0e') {
    puVar11 = (uint *)(*(int *)(param_1 + 0x198) + 0xb6c);
  }
  local_30 = &local_98;
  puVar10 = (ushort *)((int)puVar11 + 0x9a);
  puVar12 = puVar11 + 0x17;
  m = (MATRIX *)(puVar11 + 0x12);
  do {
    switch((char)puVar10[-1]) {
    case '\0':
      *puVar11 = *puVar11 | 0x8040;
      *(undefined1 *)(puVar10 + -1) = 1;
      *(uint *)(puVar10 + 9) = *(uint *)(puVar10 + 9) | 0x8000;
      *puVar11 = *puVar11 | 0x400;
      puVar10[-6] = 0x2000;
      puVar10[0x50] = 0x2000;
      puVar10[-7] = 0x1000;
      puVar10[-5] = 0x1000;
      puVar10[0x4f] = 0x1000;
      puVar10[0x51] = 0x1000;
      puVar10[-0xb] = 10;
      *(undefined1 *)((int)puVar10 + -0x21) = 0;
      *(undefined1 *)(puVar10 + -0x10) = 0;
      *(uint *)(puVar10 + 9) = *(uint *)(puVar10 + 9) | 0x400;
      local_a0.vx = 0;
      uVar3 = FUN_80015fe8();
      local_a0.vz = (uVar3 & 1) * -0x28 + 0x14;
      iVar6 = *(int *)(puVar10 + -0x33);
      if (iVar6 == -100) {
        local_a0.vx = 100;
        uVar3 = FUN_80015fe8();
        local_a0.vz = (uVar3 & 1) * -0x3c + 0x1e;
        iVar6 = *(int *)(puVar10 + -0x33);
      }
      if (iVar6 == 100) {
        local_a0.vx = -100;
        uVar3 = FUN_80015fe8();
        local_a0.vz = (uVar3 & 1) * -0x3c + 0x1e;
      }
      local_a0.vy = 0;
      RotMatrix(&local_a0,&MStack_90);
      MulMatrix(m,&MStack_90);
      FUN_8004fba0(puVar12,200,0x2000,0);
      if (DAT_800dcbc8 != 0) {
        *(undefined1 *)(puVar10 + -1) = 2;
        return;
      }
      local_38 = '\0';
    case '\x01':
      local_a0.vx = 0;
      local_a0.vy = 800;
      local_a0.vz = 0;
      ApplyMatrixSV(m,&local_a0,local_30);
      *(int *)(puVar10 + -0x1f) = *(int *)(puVar10 + -0x1f) + (int)local_98.vx;
      *(int *)(puVar10 + -0x1d) = *(int *)(puVar10 + -0x1d) + (int)local_98.vy;
      *(int *)(puVar10 + -0x1b) = *(int *)(puVar10 + -0x1b) + (int)local_98.vz;
      if (local_38 == '\0') {
        *(int *)(puVar10 + -0x1f) = *(int *)(puVar10 + -0x1f) - (int)local_98.vx;
        *(int *)(puVar10 + -0x1d) = *(int *)(puVar10 + -0x1d) - (int)local_98.vy;
        *(int *)(puVar10 + -0x1b) = *(int *)(puVar10 + -0x1b) - (int)local_98.vz;
      }
      FUN_80046f98(local_30,&SStack_58);
      *puVar10 = SStack_58.vy;
      if (*(int *)(puVar10 + -0x33) == 0) {
        uVar8 = 10000;
      }
      else {
        uVar8 = 5000;
      }
      iVar6 = FUN_8004618c(puVar12,(int)SStack_58.vy,uVar8,800);
      uVar7 = FUN_80047858(iVar6,m,&DAT_800a6b88);
      uVar7 = uVar7 & 0xffff;
      iVar9 = *(int *)(iVar6 + 0x198) + uVar7 * 0xac;
      local_70 = (short)(*(int *)(iVar9 + 0x5c) - *(int *)(puVar10 + -0x1f) >> 3);
      local_6e = (short)(*(int *)(iVar9 + 0x60) - *(int *)(puVar10 + -0x1d) >> 3);
      local_6c = (short)(*(int *)(iVar9 + 100) - *(int *)(puVar10 + -0x1b) >> 3);
      FUN_80046f98(&local_70,&SStack_60);
      if ((iVar6 != param_1) && (puVar10[-0xb] != 0)) {
        FUN_80046f44((int)SStack_60.vy,(int)SStack_58.vy,0x20,local_48);
        SStack_58.vy = local_48[0];
        FUN_80046f44((int)SStack_60.vz,(int)SStack_58.vz,0x58,local_48);
        SStack_58.vz = local_48[0];
        RotMatrix(&SStack_58,m);
        puVar10[-0xb] = puVar10[-0xb] - 1;
      }
      FUN_8004fba0(puVar12,100,0x2000,0);
      if (DAT_800dcbc8 != 0) {
        *(undefined1 *)(puVar10 + -1) = 2;
      }
      if (*(int *)(puVar10 + -0x1d) < DAT_800d5be4) {
        *(undefined1 *)(puVar10 + -1) = 2;
      }
      if (60000 < *(int *)(puVar10 + -0x1f) + 30000U) {
        *(undefined1 *)(puVar10 + -1) = 2;
        *(undefined1 *)((int)puVar10 + -0x21) = 1;
      }
      if (60000 < *(int *)(puVar10 + -0x1d) + 30000U) {
        *(undefined1 *)(puVar10 + -1) = 2;
        *(undefined1 *)((int)puVar10 + -0x21) = 1;
      }
      if (60000 < *(int *)(puVar10 + -0x1b) + 30000U) {
        *(undefined1 *)(puVar10 + -1) = 2;
        *(undefined1 *)((int)puVar10 + -0x21) = 1;
      }
      iVar6 = FUN_800470c0(puVar12,(int)(short)puVar10[-0xe],auStack_50,0xc);
      *(int *)(puVar10 + 1) = iVar6;
      if (iVar6 != 0) {
        uVar7 = FUN_80047858(iVar6,m,&DAT_800a6b88);
        uVar7 = uVar7 & 0xffff;
      }
      sVar4 = FUN_800527b4(*(undefined4 *)(puVar10 + -0x1f),*(undefined4 *)(puVar10 + -0x1b));
      iVar6 = (int)sVar4;
      if (iVar6 != 0) {
        if (iVar6 < *(int *)(puVar10 + -0x1d)) {
          iVar9 = FUN_80015fe8();
          FUN_8001bf10(iVar9 * 2 + 0x418U | 0x1a000000,
                       (int)(((ushort)SStack_58.vy + 0x800) * 0x10000) >> 0x10,m,0);
          local_a0.vy = 200;
          iVar9 = FUN_80015fe8();
          FUN_8001bf10(iVar9 * 2 + 0x418U | 0x1a000000,
                       (int)(((ushort)SStack_58.vy + 0x800) * 0x10000) >> 0x10,m,&local_a0);
          local_a0.vy = 400;
          iVar9 = FUN_80015fe8();
          FUN_8001bf10(iVar9 * 2 + 0x418U | 0x1a000000,
                       (int)(((ushort)SStack_58.vy + 0x800) * 0x10000) >> 0x10,m,&local_a0);
          local_a0.vy = 600;
          iVar9 = FUN_80015fe8();
          FUN_8001bf10(iVar9 * 2 + 0x418U | 0x1a000000,
                       (int)(((ushort)SStack_58.vy + 0x800) * 0x10000) >> 0x10,m,&local_a0);
        }
        if (((char)puVar10[-0x10] == '\0') && (iVar6 < *(int *)(puVar10 + -0x1d))) {
          *(undefined1 *)(puVar10 + -0x10) = 1;
          local_a0.vx = (short)*(undefined4 *)(puVar10 + -0x1f);
          local_a0.vz = (short)*(undefined4 *)(puVar10 + -0x1b);
          local_a0.vy = sVar4;
          iVar6 = FUN_80015fe8();
          FUN_8001bf10(iVar6 * 2 + 0x418U | 0x1a050000,0,&DAT_8009db44,&local_a0);
        }
      }
      if (*(int *)(puVar10 + 1) != 0) {
        *(undefined1 *)(puVar10 + -1) = 5;
        *(char *)((int)puVar10 + -1) = (char)uVar7;
        *(uint **)(*(int *)(puVar10 + 1) + 0x200) = puVar11;
        if (*(char *)(*(int *)(puVar10 + 1) + 8) == '!') {
          *(short *)(*(int *)(puVar10 + 1) + 0x144) = local_98.vx;
          *(short *)(*(int *)(puVar10 + 1) + 0x146) = local_98.vy;
          *(short *)(*(int *)(puVar10 + 1) + 0x148) = local_98.vz;
        }
      }
      break;
    case '\x02':
      *puVar11 = *puVar11 & 0xfffffbff;
      *(uint *)(puVar10 + 9) = *(uint *)(puVar10 + 9) & 0xfffffbff;
      if (*(char *)((int)puVar10 + -0x21) == '\0') {
        FUN_8005ba28(0x10a0001,puVar12);
      }
      puVar10[-0xb] = 0x14;
      *(undefined1 *)(puVar10 + -1) = 3;
      *(undefined1 *)((int)puVar10 + -0x21) = 0xff;
      puVar10[9] = 0;
      puVar10[10] = 0;
      puVar10[-10] = (ushort)*(undefined4 *)(puVar10 + -0x1d);
      local_a0.vx = 0;
      local_a0.vy = 400;
      local_a0.vz = 0;
      ApplyMatrixSV(m,&local_a0,&local_98);
      *(int *)(puVar10 + -0x1f) = *(int *)(puVar10 + -0x1f) + (int)local_98.vx;
      *(int *)(puVar10 + -0x1d) = *(int *)(puVar10 + -0x1d) + (int)local_98.vy;
      *(int *)(puVar10 + -0x1b) = *(int *)(puVar10 + -0x1b) + (int)local_98.vz;
    case '\x03':
      local_a0.vx = 0;
      local_a0.vy = puVar10[-0xb] * *(char *)((int)puVar10 + -0x21) * 8;
      local_a0.vz = puVar10[-0xb] * *(char *)((int)puVar10 + -0x21) * 8;
      RotMatrix(&local_a0,&MStack_90);
      MulMatrix0(m,&MStack_90,m);
      uVar3 = puVar10[-0xb];
      *(char *)((int)puVar10 + -0x21) = -*(char *)((int)puVar10 + -0x21);
      puVar10[-0xb] = uVar3 - 1;
      if (uVar3 == 0) {
        *(undefined1 *)(puVar10 + -1) = 4;
      }
      if ((_DAT_800d481c == 0x90004) && (iVar6 = FUN_80015280(puVar12,&DAT_800a7060), iVar6 == 0)) {
        *(int *)(puVar10 + -0x1d) = *(int *)(puVar10 + -0x1d) + -0xb4;
      }
      break;
    case '\x04':
      *puVar11 = 0;
      puVar10[9] = 0;
      puVar10[10] = 0;
      *(undefined1 *)(puVar10 + -1) = 0xff;
      iVar6 = *(int *)(param_1 + 0x198);
      *(int *)(puVar10 + -0x13) = iVar6 + 0x7ac;
      *(int *)(puVar10 + -3) = iVar6 + 0x764;
      *puVar11 = *puVar11 | 0x40;
      puVar10[-0xffffffff00000037] = 200;
      puVar10[-0xffffffff00000036] = 0;
      puVar10[-0xffffffff00000035] = 0x390;
      puVar10[-0xffffffff00000034] = 0;
      puVar10[-0xffffffff00000033] = 0;
      puVar10[-0xffffffff00000032] = 0;
      break;
    case '\x05':
      *puVar11 = *puVar11 & 0xfffffbff;
      puVar10[-0xb] = 0x4b;
      *(uint *)(puVar10 + 9) = *(uint *)(puVar10 + 9) & 0xfffffbff;
      if ((*(char *)(*(int *)(puVar10 + 1) + 8) == '6') ||
         (*(char *)(*(int *)(puVar10 + 1) + 8) == '#')) {
        puVar10[-0xb] = 10;
      }
      if (*(char *)(*(int *)(puVar10 + 1) + 8) == '&') {
        puVar10[-0xb] = 0;
      }
      *(undefined1 *)(puVar10 + -1) = 6;
      *(undefined1 *)((int)puVar10 + -0x21) = 0xff;
      puVar10[-10] = (ushort)*(undefined4 *)(puVar10 + -0x1d);
      iVar6 = *(int *)(*(int *)(puVar10 + 1) + 0x198) + (uint)*(byte *)((int)puVar10 + -1) * 0xac;
      local_70 = (short)*(undefined4 *)(iVar6 + 0x5c) - (short)*(undefined4 *)(puVar10 + -0x1f);
      local_6e = (short)*(undefined4 *)(iVar6 + 0x60) - (short)*(undefined4 *)(puVar10 + -0x1d);
      local_6c = (short)*(undefined4 *)(iVar6 + 100) - (short)*(undefined4 *)(puVar10 + -0x1b);
      FUN_80046f98(&local_70,&SStack_60);
      RotMatrix(&SStack_60,m);
      lVar5 = SquareRoot0((int)local_70 * (int)local_70 + (int)local_6c * (int)local_6c);
      uVar7 = SquareRoot0(lVar5 * lVar5 + (int)local_6e * (int)local_6e);
      local_a0.vy = (short)(uVar7 >> 2) * 3;
      local_a0.vx = 0;
      local_a0.vz = 0;
      ApplyMatrixSV(m,&local_a0,&local_98);
      *(int *)(puVar10 + -0x1f) = *(int *)(puVar10 + -0x1f) + (int)local_98.vx;
      *(int *)(puVar10 + -0x1d) = *(int *)(puVar10 + -0x1d) + (int)local_98.vy;
      *(int *)(puVar10 + -0x1b) = *(int *)(puVar10 + -0x1b) + (int)local_98.vz;
      FUN_8005ba28(0x10f0001,puVar12);
      iVar9 = *(int *)(*(int *)(puVar10 + 1) + 0x198) + (uint)*(byte *)((int)puVar10 + -1) * 0xac;
      iVar6 = iVar9 + 0x48;
      FUN_8002d030(iVar6,m,puVar11 + 6);
      *(int *)(puVar10 + -0x13) = iVar6;
      *(int *)(puVar10 + -3) = iVar9;
      *puVar11 = *puVar11 & 0xffffffbf;
      if ((**(uint **)(puVar10 + 1) & 0x10000) == 0) {
        local_a0.vx = (short)*(undefined4 *)(puVar10 + -0x37);
        local_a0.vy = (short)*(undefined4 *)(puVar10 + -0x35);
        local_a0.vz = (short)*(undefined4 *)(puVar10 + -0x33);
        FUN_8001bf10((**(uint **)(puVar10 + 1) >> 0x13 & 0x18) << 0x10 | 0x1bb8,
                     (int)((*puVar10 - 0x800) * 0x10000) >> 0x10,
                     (*(uint **)(puVar10 + 1))[0x66] + (uint)*(byte *)((int)puVar10 + -1) * 0xac +
                     0x48,&local_a0);
      }
    case '\x06':
      uVar3 = puVar10[-0xb];
      puVar10[-0xb] = uVar3 - 1;
      if (uVar3 == 0) {
        *(undefined1 *)(puVar10 + -1) = 4;
      }
      if (((puVar10[-0xb] & 3) == 0) && ((**(uint **)(puVar10 + 1) & 0x10000) == 0)) {
        bVar2 = *(byte *)((int)puVar10 + -1);
        uVar7 = (*(uint **)(puVar10 + 1))[0x66];
        local_a0.vx = (short)*(undefined4 *)(puVar10 + -0x37);
        local_a0.vy = (short)*(undefined4 *)(puVar10 + -0x35);
        local_a0.vz = (short)*(undefined4 *)(puVar10 + -0x33);
        iVar6 = FUN_80015fe8();
        FUN_8001bf10((**(uint **)(puVar10 + 1) >> 0x13 & 0x18) << 0x10 | (iVar6 + 0x200) * 2,
                     (int)((*puVar10 - 0x800) * 0x10000) >> 0x10,uVar7 + (uint)bVar2 * 0xac + 0x48,
                     &local_a0);
      }
    }
    puVar10 = puVar10 + 0xac;
    puVar12 = puVar12 + 0x56;
    m = (MATRIX *)(m[10].t + 1);
    puVar11 = puVar11 + 0x56;
    local_40 = local_40 + -1;
    if (local_40 == 0) {
      return;
    }
  } while( true );
}


