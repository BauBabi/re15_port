/* FUN_8001c224 @ 0x8001c224  (Ghidra headless, raw MIPS overlay) */

uint FUN_8001c224(uint param_1,undefined2 param_2,undefined4 *param_3,undefined4 *param_4,
                 undefined4 *param_5)

{
  ushort uVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  undefined4 *puVar9;
  undefined4 *puVar10;
  ushort *puVar11;
  uint uVar12;
  int iVar13;
  ushort *puVar14;
  uint uVar15;
  
  uVar12 = 0x60;
  iVar4 = (param_1 >> 0x18) * 4;
  uVar15 = param_1 >> 0x10 & 0xff;
  puVar11 = *(ushort **)(&DAT_800d4cd8 + iVar4);
  puVar14 = (ushort *)
            (*(int *)(&DAT_800d4e18 + iVar4) +
            (uint)*(ushort *)((param_1 >> 0x10 & 7) * 2 + *(int *)(&DAT_800d4e18 + iVar4)) * 4);
  uVar1 = *puVar14;
  uVar2 = *puVar11;
  iVar4 = 0x2e04;
  while (uVar12 = uVar12 - 1, *(short *)((int)&DAT_800d8d08 + iVar4) != 0) {
    iVar4 = iVar4 + -0x7c;
    if (uVar12 == 0) {
      return 0xff;
    }
  }
  *(undefined2 *)((int)&DAT_800d8d08 + iVar4) = 0xa003;
  (&DAT_800d8d0a)[iVar4] = 0;
  (&DAT_800d8d0b)[iVar4] = 0;
  *(uint *)(&DAT_800d8d0c + iVar4) = uVar15 * 0x10000 + (param_1 >> 0x18);
  uVar3 = puVar11[5];
  *(undefined2 *)(&UNK_800d8d12 + iVar4) = param_2;
  *(undefined4 *)(&UNK_800d8d14 + iVar4) = 0;
  *(ushort *)(&UNK_800d8d10 + iVar4) = (ushort)(byte)uVar3;
  *(uint *)(&UNK_800d8d18 + iVar4) = (uint)puVar11[3] << 0x10;
  if (param_4 == (undefined4 *)0x0) {
    *(undefined4 *)(&UNK_800d8d1c + iVar4) = 0;
    *(undefined4 *)(&UNK_800d8d20 + iVar4) = 0;
  }
  else {
    *(undefined4 *)(&UNK_800d8d1c + iVar4) = *param_4;
    *(undefined4 *)(&UNK_800d8d20 + iVar4) = param_4[1];
  }
  uVar3 = puVar11[2];
  *(uint *)(&DAT_800d8d28 + iVar4) = param_1 << 0x10;
  *(undefined4 *)(&UNK_800d8d30 + iVar4) = 0x10000;
  *(undefined4 *)(&DAT_800d8d24 + iVar4) = 0;
  *(undefined4 *)(&UNK_800d8d2c + iVar4) = 0;
  *(undefined4 *)(&DAT_800d8d34 + iVar4) = 0;
  *(undefined4 *)(&DAT_800d8d38 + iVar4) = 0;
  *(ushort *)(&UNK_800d8d22 + iVar4) = uVar3 + (short)(uVar15 >> 3) * 0x40;
  uVar5 = param_3[1];
  uVar6 = param_3[2];
  uVar7 = param_3[3];
  *(undefined4 *)(&UNK_800d8d3c + iVar4) = *param_3;
  *(undefined4 *)(&UNK_800d8d40 + iVar4) = uVar5;
  *(undefined4 *)(&UNK_800d8d44 + iVar4) = uVar6;
  *(undefined4 *)(&UNK_800d8d48 + iVar4) = uVar7;
  uVar5 = param_3[5];
  uVar6 = param_3[6];
  uVar7 = param_3[7];
  *(undefined4 *)(&UNK_800d8d4c + iVar4) = param_3[4];
  *(undefined4 *)(&UNK_800d8d50 + iVar4) = uVar5;
  *(undefined4 *)(&UNK_800d8d54 + iVar4) = uVar6;
  *(undefined4 *)(&UNK_800d8d58 + iVar4) = uVar7;
  *(ushort **)(&UNK_800d8d60 + iVar4) = puVar11 + 4;
  *(ushort **)(&UNK_800d8d64 + iVar4) = puVar11 + (uint)uVar2 * 4 + 4;
  *(undefined4 **)((int)&DAT_800d8d5c + iVar4) = param_3;
  *(ushort **)(&UNK_800d8d68 + iVar4) = puVar14 + 4;
  *(undefined4 *)(&DAT_800d8cf0 + iVar4) = *(undefined4 *)(puVar14 + 4);
  *(undefined4 *)(&DAT_800d8cf4 + iVar4) = *(undefined4 *)(*(int *)(&UNK_800d8d68 + iVar4) + 4);
  *(undefined4 *)(&DAT_800d8cf8 + iVar4) = *(undefined4 *)(*(int *)(&UNK_800d8d68 + iVar4) + 8);
  *(undefined4 *)(&DAT_800d8cfc + iVar4) = *(undefined4 *)(*(int *)(&UNK_800d8d68 + iVar4) + 0xc);
  *(undefined4 *)(&DAT_800d8d00 + iVar4) = *(undefined4 *)(*(int *)(&UNK_800d8d68 + iVar4) + 0x10);
  *(undefined4 *)(&UNK_800d8d04 + iVar4) = *(undefined4 *)(*(int *)(&UNK_800d8d68 + iVar4) + 0x14);
  *(undefined4 *)(&DAT_800d8d34 + iVar4) = *param_5;
  iVar13 = uVar1 - 1;
  *(undefined2 *)(&DAT_800d8d38 + iVar4) = *(undefined2 *)(param_5 + 1);
  if (iVar13 != 0) {
    uVar15 = uVar12;
    do {
      if (uVar15 == 0) {
        return 0xfe;
      }
      iVar4 = uVar15 * 0x7c;
      while( true ) {
        uVar15 = uVar15 - 1;
        if (*(short *)(&UNK_800d8c8c + iVar4) == 0) break;
        iVar4 = iVar4 + -0x7c;
        if (uVar15 == 0) {
          return 0xfe;
        }
      }
      puVar10 = (undefined4 *)(&UNK_800d8c8c + iVar4);
      *(undefined2 *)(&UNK_800d8c8c + iVar4) = 0xa003;
      (&UNK_800d8c8f)[iVar4] = 0;
      puVar9 = (undefined4 *)(&DAT_800d8d08 + uVar12 * 0x3e);
      do {
        uVar5 = puVar9[1];
        uVar6 = puVar9[2];
        uVar7 = puVar9[3];
        *puVar10 = *puVar9;
        puVar10[1] = uVar5;
        puVar10[2] = uVar6;
        puVar10[3] = uVar7;
        puVar9 = puVar9 + 4;
        puVar10 = puVar10 + 4;
      } while (puVar9 != (undefined4 *)(&UNK_800d8d68 + uVar12 * 0x7c));
      *puVar10 = *puVar9;
      (&UNK_800d8c8e)[iVar4] = (char)uVar12;
      puVar11 = puVar14 + 2;
      iVar8 = iVar13;
      do {
        iVar8 = iVar8 + -1;
        puVar11 = puVar11 + (uint)*puVar11 * 0xc + 2;
      } while (iVar8 != 0);
      *(ushort **)((int)&DAT_800d8cec + iVar4) = puVar11 + 2;
      *(undefined4 *)(&UNK_800d8c74 + iVar4) = *(undefined4 *)(puVar11 + 2);
      *(undefined4 *)(&UNK_800d8c78 + iVar4) =
           *(undefined4 *)(*(int *)((int)&DAT_800d8cec + iVar4) + 4);
      *(undefined4 *)(&UNK_800d8c7c + iVar4) =
           *(undefined4 *)(*(int *)((int)&DAT_800d8cec + iVar4) + 8);
      *(undefined4 *)(&UNK_800d8c80 + iVar4) =
           *(undefined4 *)(*(int *)((int)&DAT_800d8cec + iVar4) + 0xc);
      *(undefined4 *)(&UNK_800d8c84 + iVar4) =
           *(undefined4 *)(*(int *)((int)&DAT_800d8cec + iVar4) + 0x10);
      *(undefined4 *)(&UNK_800d8c88 + iVar4) =
           *(undefined4 *)(*(int *)((int)&DAT_800d8cec + iVar4) + 0x14);
      *(undefined4 *)((int)&DAT_800d8cb8 + iVar4) = *param_5;
      iVar13 = iVar13 + -1;
      *(undefined2 *)((int)&DAT_800d8cbc + iVar4) = *(undefined2 *)(param_5 + 1);
    } while (iVar13 != 0);
  }
  return uVar12 & 0xff;
}


