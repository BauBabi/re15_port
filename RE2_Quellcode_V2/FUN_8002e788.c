/* FUN_8002e788 @ 0x8002e788  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e788(int param_1)

{
  ushort uVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  uint uVar14;
  
  puVar13 = *(undefined4 **)(param_1 + 0x198);
  puVar12 = *(undefined4 **)(param_1 + 0x1a4);
  uVar14 = (uint)*(byte *)(param_1 + 0x107);
  puVar11 = puVar12 + 0x2a;
  puVar10 = puVar13 + 0x2a;
  do {
    *puVar12 = *puVar13;
    uVar5 = puVar10[-0x23];
    uVar8 = puVar10[-0x22];
    uVar9 = puVar10[-0x21];
    puVar11[-0x24] = puVar10[-0x24];
    puVar11[-0x23] = uVar5;
    puVar11[-0x22] = uVar8;
    puVar11[-0x21] = uVar9;
    uVar5 = puVar10[-0x1f];
    uVar8 = puVar10[-0x1e];
    uVar9 = puVar10[-0x1d];
    puVar11[-0x20] = puVar10[-0x20];
    puVar11[-0x1f] = uVar5;
    puVar11[-0x1e] = uVar8;
    puVar11[-0x1d] = uVar9;
    uVar5 = puVar10[-0x17];
    uVar8 = puVar10[-0x16];
    uVar9 = puVar10[-0x15];
    puVar11[-0x18] = puVar10[-0x18];
    puVar11[-0x17] = uVar5;
    puVar11[-0x16] = uVar8;
    puVar11[-0x15] = uVar9;
    uVar6 = puVar10[-0x13];
    uVar5 = puVar10[-0x12];
    uVar8 = puVar10[-0x11];
    puVar11[-0x14] = puVar10[-0x14];
    puVar11[-0x13] = uVar6;
    puVar11[-0x12] = uVar5;
    puVar11[-0x11] = uVar8;
    puVar11[-0xd] = puVar10[-0xd];
    puVar11[-5] = puVar10[-5];
    *(undefined2 *)(puVar11 + -9) = *(undefined2 *)(puVar10 + -9);
    *(undefined1 *)((int)puVar11 + -0x2f) = *(undefined1 *)((int)puVar10 + -0x2f);
    *(undefined1 *)((int)puVar11 + -0x2e) = *(undefined1 *)((int)puVar10 + -0x2e);
    uVar1 = *(ushort *)((int)puVar10 + -0x22);
    *(ushort *)((int)puVar11 + -0x22) = uVar1;
    uVar2 = (int)puVar10 - 0x19U & 3;
    uVar7 = (uint)(puVar10 + -7) & 3;
    uVar4 = (*(int *)(((int)puVar10 - 0x19U) - uVar2) << (3 - uVar2) * 8 |
            (uint)uVar1 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -7) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar10 - 0x15U & 3;
    uVar7 = (uint)(puVar10 + -6) & 3;
    uVar6 = (*(int *)(((int)puVar10 - 0x15U) - uVar2) << (3 - uVar2) * 8 |
            uVar6 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -6) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar11 - 0x19U & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x19U) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar4 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -7) & 3;
    puVar3 = (uint *)((int)(puVar11 + -7) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar4 << uVar2 * 8;
    uVar2 = (int)puVar11 - 0x15U & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x15U) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar6 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -6) & 3;
    puVar3 = (uint *)((int)(puVar11 + -6) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar6 << uVar2 * 8;
    uVar2 = (int)puVar10 - 0x29U & 3;
    uVar7 = (uint)(puVar10 + -0xb) & 3;
    uVar4 = (*(int *)(((int)puVar10 - 0x29U) - uVar2) << (3 - uVar2) * 8 |
            uVar4 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -0xb) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar10 - 0x25U & 3;
    uVar7 = (uint)(puVar10 + -10) & 3;
    uVar6 = (*(int *)(((int)puVar10 - 0x25U) - uVar2) << (3 - uVar2) * 8 |
            uVar6 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -10) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar11 - 0x29U & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x29U) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar4 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -0xb) & 3;
    puVar3 = (uint *)((int)(puVar11 + -0xb) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar4 << uVar2 * 8;
    uVar2 = (int)puVar11 - 0x25U & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x25U) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar6 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -10) & 3;
    puVar3 = (uint *)((int)(puVar11 + -10) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar6 << uVar2 * 8;
    uVar2 = (int)puVar10 - 0x3dU & 3;
    uVar7 = (uint)(puVar10 + -0x10) & 3;
    uVar4 = (*(int *)(((int)puVar10 - 0x3dU) - uVar2) << (3 - uVar2) * 8 |
            uVar4 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -0x10) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar10 - 0x39U & 3;
    uVar7 = (uint)(puVar10 + -0xf) & 3;
    uVar6 = (*(int *)(((int)puVar10 - 0x39U) - uVar2) << (3 - uVar2) * 8 |
            uVar6 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -0xf) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar11 - 0x3dU & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x3dU) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar4 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -0x10) & 3;
    puVar3 = (uint *)((int)(puVar11 + -0x10) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar4 << uVar2 * 8;
    uVar2 = (int)puVar11 - 0x39U & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x39U) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar6 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -0xf) & 3;
    puVar3 = (uint *)((int)(puVar11 + -0xf) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar6 << uVar2 * 8;
    uVar2 = (int)puVar10 - 0x6dU & 3;
    uVar7 = (uint)(puVar10 + -0x1c) & 3;
    uVar4 = (*(int *)(((int)puVar10 - 0x6dU) - uVar2) << (3 - uVar2) * 8 |
            uVar4 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -0x1c) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar10 - 0x69U & 3;
    uVar7 = (uint)(puVar10 + -0x1b) & 3;
    uVar7 = (*(int *)(((int)puVar10 - 0x69U) - uVar2) << (3 - uVar2) * 8 |
            uVar6 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar7) * 8 |
            *(uint *)((int)(puVar10 + -0x1b) - uVar7) >> uVar7 * 8;
    uVar2 = (int)puVar11 - 0x6dU & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x6dU) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar4 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -0x1c) & 3;
    puVar3 = (uint *)((int)(puVar11 + -0x1c) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar4 << uVar2 * 8;
    uVar2 = (int)puVar11 - 0x69U & 3;
    puVar3 = (uint *)(((int)puVar11 - 0x69U) - uVar2);
    *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar7 >> (3 - uVar2) * 8;
    uVar2 = (uint)(puVar11 + -0x1b) & 3;
    puVar3 = (uint *)((int)(puVar11 + -0x1b) - uVar2);
    *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar7 << uVar2 * 8;
    puVar11[-4] = puVar10[-4];
    puVar13 = puVar13 + 0x2b;
    puVar11[-3] = puVar10[-3];
    uVar14 = uVar14 - 1;
    puVar11[-2] = puVar10[-2];
    puVar12 = puVar12 + 0x2b;
    puVar11[-1] = puVar10[-1];
    uVar5 = *puVar10;
    puVar10 = puVar10 + 0x2b;
    *puVar11 = uVar5;
    puVar11 = puVar11 + 0x2b;
  } while (uVar14 != 0);
  return;
}


