undefined4 FUN_8001e5b0(int param_1,undefined4 param_2)

{
  byte bVar1;
  ushort uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  byte *pbVar10;
  undefined4 *puVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  ushort *puVar14;
  undefined4 *puVar15;
  undefined4 *puVar16;
  undefined4 *puVar17;
  uint uVar18;
  uint *puVar19;
  int iVar20;
  ushort uVar21;
  undefined4 local_b8 [15];
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined2 local_5c;
  undefined2 local_5a;
  undefined2 local_58 [2];
  undefined4 local_54;
  undefined4 local_50;
  int local_48;
  uint *local_40;
  ushort *local_38;
  undefined4 *local_30;
  
  local_48 = 0;
  uVar18 = (uint)*(byte *)(param_1 + 0x83);
  puVar16 = *(undefined4 **)(param_1 + 0x188);
  puVar14 = *(ushort **)(param_1 + 0x84);
  iVar20 = *(int *)(param_1 + 0x10);
  local_38 = puVar14 + 4;
  uVar21 = puVar14[2];
  uVar2 = *puVar14;
  local_7c = 0x1000;
  local_78 = 0;
  local_74 = 0x1000;
  local_70 = 0;
  local_6c = 0x1000;
  local_68 = 0;
  local_64 = 0;
  local_60 = 0;
  local_b8[5] = 0x1000;
  local_b8[6] = 0;
  local_b8[7] = 0x1000;
  local_b8[8] = 0;
  local_b8[9] = 0x1000;
  local_b8[10] = 0;
  local_b8[0xb] = 0;
  local_b8[0xc] = 0;
  local_b8[0] = 0;
  local_54 = 0x808080;
  local_50 = 0;
  local_5c = 0;
  local_5a = 0;
  local_58[0] = 0;
  puVar16[0x1b] = param_1 + 0x20;
  puVar16[0x24] = param_1;
  puVar19 = (uint *)((uVar2 & 0xfffc) + (int)puVar14);
  local_40 = puVar19;
  if (uVar18 != 0) {
    local_30 = (undefined4 *)local_58;
    puVar15 = puVar16 + 0x24;
    puVar14 = puVar14 + 6;
    puVar17 = puVar16;
    do {
      uVar18 = uVar18 - 1;
      uVar12 = puVar15[-9];
      uVar13 = *puVar15;
      puVar9 = local_b8;
      puVar3 = puVar17 + 1;
      do {
        puVar11 = puVar3;
        puVar8 = puVar9;
        uVar4 = puVar8[1];
        uVar5 = puVar8[2];
        uVar6 = puVar8[3];
        *puVar11 = *puVar8;
        puVar11[1] = uVar4;
        puVar11[2] = uVar5;
        puVar11[3] = uVar6;
        puVar9 = puVar8 + 4;
        puVar3 = puVar11 + 4;
      } while (puVar9 != local_30);
      uVar4 = puVar8[5];
      uVar5 = puVar8[6];
      puVar11[4] = *puVar9;
      puVar11[5] = uVar4;
      puVar11[6] = uVar5;
      puVar15[-9] = uVar12;
      *puVar15 = uVar13;
      *puVar17 = 1;
      *(undefined2 *)(puVar15 + -0xf) = 0;
      puVar15[-0x22] = iVar20;
      puVar15[-0x20] = iVar20 + 0x1c;
      *(undefined1 *)(puVar15 + -7) = (undefined1)local_48;
      uVar12 = FUN_80025940(puVar17 + 1,param_2);
      param_2 = FUN_80025a98(puVar17 + 1,uVar12);
      uVar2 = *local_38;
      puVar15[-0x19] = (int)(short)uVar2;
      *(ushort *)(puVar15 + -0x16) = uVar2;
      uVar2 = puVar14[-1];
      local_38 = local_38 + 3;
      puVar15[-0x18] = (int)(short)uVar2;
      *(ushort *)((int)puVar15 + -0x56) = uVar2;
      uVar2 = *puVar14;
      puVar15[-0x17] = (int)(short)uVar2;
      *(ushort *)(puVar15 + -0x15) = uVar2;
      puVar14 = puVar14 + 3;
      if (uVar21 == 0) {
        puVar15[-9] = &DAT_80072d4c;
        *puVar15 = 0;
      }
      else {
        pbVar10 = (byte *)((int)local_40 + (*puVar19 >> 0x10));
        uVar7 = *puVar19 & 0xffff;
        if (uVar7 != 0) {
          do {
            uVar7 = uVar7 - 1;
            puVar16[(uint)*pbVar10 * 0x2b + 0x1b] = puVar17 + 0x10;
            bVar1 = *pbVar10;
            pbVar10 = pbVar10 + 1;
            puVar16[(uint)bVar1 * 0x2b + 0x24] = puVar17;
          } while (uVar7 != 0);
        }
        puVar19 = puVar19 + 1;
        uVar21 = uVar21 - 1;
      }
      puVar15 = puVar15 + 0x2b;
      puVar17 = puVar17 + 0x2b;
      iVar20 = iVar20 + 0x38;
      local_48 = local_48 + 1;
    } while (uVar18 != 0);
  }
  return param_2;
}
