/* FUN_8001dc30 @ 0x8001dc30  (Ghidra headless, raw MIPS overlay) */

void FUN_8001dc30(undefined4 param_1,undefined4 param_2,uint param_3,uint param_4)

{
  undefined1 *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  
  uVar2 = DAT_800dcbd0;
  puVar1 = (undefined1 *)(DAT_800dcbd0 + 2);
  *(undefined2 *)(DAT_800dcbd0 + 0x18) = *(undefined2 *)(DAT_800dcbd0 + 0x12);
  *(undefined1 *)(uVar2 + 0x21) = *puVar1;
  uVar8 = DAT_800dcbd0;
  *(ushort *)(uVar2 + 0x2a) = *(ushort *)(uVar2 + 0x2a) | *(ushort *)(uVar2 + 0x14);
  *(undefined1 *)(uVar8 + 0x20) =
       *(undefined1 *)(*(int *)(uVar8 + 0x70) + (uint)*(byte *)(uVar8 + 0x21) * 8 + 2);
  *(char *)(DAT_800dcbd0 + 0x1f) = *(char *)(DAT_800dcbd0 + 0x1f) + *(char *)(DAT_800dcbd0 + 0xb);
  uVar4 = DAT_800dcbd0;
  uVar5 = (uint)*(byte *)(DAT_800dcbd0 + 0x1f) * 0x18 + *(uint *)(DAT_800dcbd0 + 0x78);
  uVar2 = uVar5 + 3 & 3;
  uVar9 = uVar5 & 3;
  uVar6 = (*(int *)((uVar5 + 3) - uVar2) << (3 - uVar2) * 8 |
          *(uint *)(DAT_800dcbd0 + 0x78) & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar9) * 8 |
          *(uint *)(uVar5 - uVar9) >> uVar9 * 8;
  uVar2 = uVar5 + 7 & 3;
  uVar9 = uVar5 + 4 & 3;
  uVar7 = (*(int *)((uVar5 + 7) - uVar2) << (3 - uVar2) * 8 | uVar8 & 0xffffffffU >> (uVar2 + 1) * 8
          ) & -1 << (4 - uVar9) * 8 | *(uint *)((uVar5 + 4) - uVar9) >> uVar9 * 8;
  uVar2 = uVar5 + 0xb & 3;
  uVar9 = uVar5 + 8 & 3;
  uVar8 = (*(int *)((uVar5 + 0xb) - uVar2) << (3 - uVar2) * 8 |
          param_3 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar9) * 8 |
          *(uint *)((uVar5 + 8) - uVar9) >> uVar9 * 8;
  uVar2 = uVar5 + 0xf & 3;
  uVar9 = uVar5 + 0xc & 3;
  uVar9 = (*(int *)((uVar5 + 0xf) - uVar2) << (3 - uVar2) * 8 |
          param_4 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar9) * 8 |
          *(uint *)((uVar5 + 0xc) - uVar9) >> uVar9 * 8;
  uVar2 = DAT_800dcbd0 + 3 & 3;
  puVar3 = (uint *)((DAT_800dcbd0 + 3) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar6 >> (3 - uVar2) * 8;
  uVar2 = uVar4 & 3;
  *(uint *)(uVar4 - uVar2) =
       *(uint *)(uVar4 - uVar2) & 0xffffffffU >> (4 - uVar2) * 8 | uVar6 << uVar2 * 8;
  uVar2 = uVar4 + 7 & 3;
  puVar3 = (uint *)((uVar4 + 7) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar7 >> (3 - uVar2) * 8;
  uVar2 = uVar4 + 4 & 3;
  puVar3 = (uint *)((uVar4 + 4) - uVar2);
  *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar7 << uVar2 * 8;
  uVar2 = uVar4 + 0xb & 3;
  puVar3 = (uint *)((uVar4 + 0xb) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar8 >> (3 - uVar2) * 8;
  uVar2 = uVar4 + 8 & 3;
  puVar3 = (uint *)((uVar4 + 8) - uVar2);
  *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar8 << uVar2 * 8;
  uVar2 = uVar4 + 0xf & 3;
  puVar3 = (uint *)((uVar4 + 0xf) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar9 >> (3 - uVar2) * 8;
  uVar2 = uVar4 + 0xc & 3;
  puVar3 = (uint *)((uVar4 + 0xc) - uVar2);
  *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar9 << uVar2 * 8;
  uVar2 = uVar5 + 0x13 & 3;
  uVar9 = uVar5 + 0x10 & 3;
  uVar8 = (*(int *)((uVar5 + 0x13) - uVar2) << (3 - uVar2) * 8 |
          uVar6 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar9) * 8 |
          *(uint *)((uVar5 + 0x10) - uVar9) >> uVar9 * 8;
  uVar2 = uVar5 + 0x17 & 3;
  uVar9 = uVar5 + 0x14 & 3;
  uVar9 = (*(int *)((uVar5 + 0x17) - uVar2) << (3 - uVar2) * 8 |
          uVar7 & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar9) * 8 |
          *(uint *)((uVar5 + 0x14) - uVar9) >> uVar9 * 8;
  uVar2 = uVar4 + 0x13 & 3;
  puVar3 = (uint *)((uVar4 + 0x13) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar8 >> (3 - uVar2) * 8;
  uVar2 = uVar4 + 0x10 & 3;
  puVar3 = (uint *)((uVar4 + 0x10) - uVar2);
  *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar8 << uVar2 * 8;
  uVar2 = uVar4 + 0x17 & 3;
  puVar3 = (uint *)((uVar4 + 0x17) - uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | uVar9 >> (3 - uVar2) * 8;
  uVar2 = uVar4 + 0x14 & 3;
  puVar3 = (uint *)((uVar4 + 0x14) - uVar2);
  *puVar3 = *puVar3 & 0xffffffffU >> (4 - uVar2) * 8 | uVar9 << uVar2 * 8;
  return;
}


