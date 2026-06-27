uint FUN_80019700(uint param_1,undefined2 param_2,uint *param_3,uint *param_4)

{
  ushort uVar1;
  ushort uVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  ushort *puVar10;
  uint *puVar11;
  uint uVar12;
  uint *puVar13;
  uint uVar14;
  uint uVar15;
  ushort *puVar16;
  ushort *puVar17;
  
  uVar14 = 0;
  iVar8 = (param_1 >> 0x18) * 4;
  puVar16 = *(ushort **)(&DAT_800b2248 + iVar8);
  puVar17 = (ushort *)
            (*(int *)(&DAT_800b22d4 + iVar8) +
            (uint)*(ushort *)((param_1 >> 0x10 & 7) * 2 + *(int *)(&DAT_800b22d4 + iVar8)) * 4);
  uVar15 = (uint)*puVar17;
  uVar1 = *puVar16;
  puVar17 = puVar17 + 2;
  do {
    if (0x5f < uVar14) {
      return 0xff;
    }
    puVar6 = (uint *)(&DAT_800a73b8 + uVar14 * 0x42);
    while ((char)puVar6[0x1b] != '\0') {
      uVar14 = uVar14 + 1;
      puVar6 = puVar6 + 0x21;
      if (0x5f < uVar14) {
        return 0xff;
      }
    }
    *(undefined1 *)(puVar6 + 0x1b) = 3;
    *(char *)(puVar6 + 0x1c) = (char)(param_1 >> 0x18);
    *(char *)((int)puVar6 + 0x71) = (char)(param_1 >> 0x10);
    *(short *)((int)puVar6 + 0x72) = (short)param_1;
    *(undefined2 *)((int)puVar6 + 0x2e) = param_2;
    if (param_4 == (uint *)0x0) {
      puVar6[0x12] = 0;
      puVar6[0x11] = 0;
      puVar6[0x10] = 0;
      puVar6[0x1d] = (uint)param_3;
    }
    else {
      uVar7 = param_4[1];
      uVar9 = param_4[2];
      uVar12 = param_4[3];
      puVar6[0x10] = *param_4;
      puVar6[0x11] = uVar7;
      puVar6[0x12] = uVar9;
      puVar6[0x13] = uVar12;
      puVar6[0x1d] = (uint)param_3;
    }
    uVar7 = param_3[1];
    uVar9 = param_3[2];
    uVar12 = param_3[3];
    puVar6[0x13] = *param_3;
    puVar6[0x14] = uVar7;
    puVar6[0x15] = uVar9;
    puVar6[0x16] = uVar12;
    uVar7 = param_3[5];
    uVar9 = param_3[6];
    uVar12 = param_3[7];
    puVar6[0x17] = param_3[4];
    puVar6[0x18] = uVar7;
    puVar6[0x19] = uVar9;
    puVar6[0x1a] = uVar12;
    puVar6[0xd] = 0;
    puVar6[0xe] = 0;
    puVar6[0xf] = 0;
    *(undefined2 *)(puVar6 + 10) = 0;
    *(undefined2 *)((int)puVar6 + 0x2a) = 0;
    *(undefined2 *)(puVar6 + 0xb) = 0;
    uVar9 = 1;
    *(ushort *)((int)puVar6 + 0x32) = puVar16[2] + (short)((param_1 >> 0x10 & 0xff) >> 3) * 0x40;
    uVar2 = puVar16[3];
    puVar6[0x1e] = (uint)(puVar16 + 4);
    *(ushort *)(puVar6 + 0xc) = uVar2;
    uVar2 = puVar16[5];
    uVar7 = (uint)(byte)uVar2;
    *(undefined1 *)((int)puVar6 + 0x6e) = 1;
    puVar6[0x1f] = (uint)(puVar16 + (uint)uVar1 * 4 + 4);
    *(byte *)((int)puVar6 + 0x6d) = (byte)uVar2;
    puVar10 = puVar17;
    if (1 < uVar15) {
      do {
        uVar9 = uVar9 + 1;
        uVar7 = (uint)*puVar10 * 0x28 + 4;
        puVar10 = puVar10 + (uint)*puVar10 * 0x14 + 2;
      } while (uVar9 < uVar15);
    }
    puVar11 = (uint *)(puVar10 + 2);
    uVar12 = (uint)puVar11 & 3;
    *(undefined1 *)((int)puVar6 + 0x6f) = 0;
    puVar6[0x20] = (uint)puVar11;
    puVar13 = puVar11;
    if (uVar12 == 0) {
      do {
        uVar12 = *puVar13;
        uVar7 = puVar13[1];
        uVar9 = puVar13[2];
        uVar5 = puVar13[3];
        *puVar6 = uVar12;
        puVar6[1] = uVar7;
        puVar6[2] = uVar9;
        puVar6[3] = uVar5;
        puVar13 = puVar13 + 4;
        puVar6 = puVar6 + 4;
      } while (puVar13 != (uint *)(puVar10 + 0x12));
    }
    else {
      do {
        uVar5 = (int)puVar13 + 3U & 3;
        uVar3 = (uint)puVar13 & 3;
        uVar12 = (*(int *)(((int)puVar13 + 3U) - uVar5) << (3 - uVar5) * 8 |
                 uVar12 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
                 *(uint *)((int)puVar13 - uVar3) >> uVar3 * 8;
        uVar5 = (int)puVar13 + 7U & 3;
        uVar3 = (uint)(puVar13 + 1) & 3;
        uVar7 = (*(int *)(((int)puVar13 + 7U) - uVar5) << (3 - uVar5) * 8 |
                uVar7 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
                *(uint *)((int)(puVar13 + 1) - uVar3) >> uVar3 * 8;
        uVar5 = (int)puVar13 + 0xbU & 3;
        uVar3 = (uint)(puVar13 + 2) & 3;
        puVar11 = (uint *)((*(int *)(((int)puVar13 + 0xbU) - uVar5) << (3 - uVar5) * 8 |
                           (uint)puVar11 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
                          *(uint *)((int)(puVar13 + 2) - uVar3) >> uVar3 * 8);
        uVar5 = (int)puVar13 + 0xfU & 3;
        uVar3 = (uint)(puVar13 + 3) & 3;
        uVar9 = (*(int *)(((int)puVar13 + 0xfU) - uVar5) << (3 - uVar5) * 8 |
                uVar9 & 0xffffffffU >> (uVar5 + 1) * 8) & -1 << (4 - uVar3) * 8 |
                *(uint *)((int)(puVar13 + 3) - uVar3) >> uVar3 * 8;
        uVar5 = (int)puVar6 + 3U & 3;
        puVar4 = (uint *)(((int)puVar6 + 3U) - uVar5);
        *puVar4 = *puVar4 & -1 << (uVar5 + 1) * 8 | uVar12 >> (3 - uVar5) * 8;
        *puVar6 = uVar12;
        uVar5 = (int)puVar6 + 7U & 3;
        puVar4 = (uint *)(((int)puVar6 + 7U) - uVar5);
        *puVar4 = *puVar4 & -1 << (uVar5 + 1) * 8 | uVar7 >> (3 - uVar5) * 8;
        puVar6[1] = uVar7;
        uVar5 = (int)puVar6 + 0xbU & 3;
        puVar4 = (uint *)(((int)puVar6 + 0xbU) - uVar5);
        *puVar4 = *puVar4 & -1 << (uVar5 + 1) * 8 | (uint)puVar11 >> (3 - uVar5) * 8;
        puVar6[2] = (uint)puVar11;
        uVar5 = (int)puVar6 + 0xfU & 3;
        puVar4 = (uint *)(((int)puVar6 + 0xfU) - uVar5);
        *puVar4 = *puVar4 & -1 << (uVar5 + 1) * 8 | uVar9 >> (3 - uVar5) * 8;
        puVar6[3] = uVar9;
        puVar13 = puVar13 + 4;
        puVar6 = puVar6 + 4;
      } while (puVar13 != (uint *)(puVar10 + 0x12));
    }
    uVar9 = (int)puVar13 + 3U & 3;
    uVar5 = (uint)puVar13 & 3;
    uVar5 = (*(int *)(((int)puVar13 + 3U) - uVar9) << (3 - uVar9) * 8 |
            uVar12 & 0xffffffffU >> (uVar9 + 1) * 8) & -1 << (4 - uVar5) * 8 |
            *(uint *)((int)puVar13 - uVar5) >> uVar5 * 8;
    uVar9 = (int)puVar13 + 7U & 3;
    uVar12 = (uint)(puVar13 + 1) & 3;
    uVar9 = (*(int *)(((int)puVar13 + 7U) - uVar9) << (3 - uVar9) * 8 |
            uVar7 & 0xffffffffU >> (uVar9 + 1) * 8) & -1 << (4 - uVar12) * 8 |
            *(uint *)((int)(puVar13 + 1) - uVar12) >> uVar12 * 8;
    uVar7 = (int)puVar6 + 3U & 3;
    puVar13 = (uint *)(((int)puVar6 + 3U) - uVar7);
    *puVar13 = *puVar13 & -1 << (uVar7 + 1) * 8 | uVar5 >> (3 - uVar7) * 8;
    *puVar6 = uVar5;
    uVar7 = (int)puVar6 + 7U & 3;
    puVar13 = (uint *)(((int)puVar6 + 7U) - uVar7);
    *puVar13 = *puVar13 & -1 << (uVar7 + 1) * 8 | uVar9 >> (3 - uVar7) * 8;
    uVar15 = uVar15 - 1;
    puVar6[1] = uVar9;
  } while (uVar15 != 0);
  return uVar14 & 0xff;
}
