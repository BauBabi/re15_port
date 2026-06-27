/* FUN_8001dd2c @ 0x8001dd2c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001dd2c(undefined4 param_1,undefined4 param_2,uint param_3,uint param_4)

{
  ushort *puVar1;
  ushort *puVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  
  uVar3 = DAT_800dcbd0;
  puVar1 = (ushort *)(DAT_800dcbd0 + 0x2a);
  puVar2 = (ushort *)(DAT_800dcbd0 + 0x14);
  uVar10 = (uint)*(ushort *)(DAT_800dcbd0 + 0x12);
  *(ushort *)(DAT_800dcbd0 + 0x18) = *(ushort *)(DAT_800dcbd0 + 0x12);
  *(ushort *)(uVar3 + 0x2a) = *puVar1 | *puVar2;
  iVar6 = FUN_80015fe8();
  iVar8 = *(ushort *)(DAT_800dcbd0 + 0x16) + 1;
  if (iVar8 == 0) {
    trap(0x1c00);
  }
  if ((iVar8 == -1) && (iVar6 == -0x80000000)) {
    trap(0x1800);
  }
  *(char *)(DAT_800dcbd0 + 0x21) = *(char *)(DAT_800dcbd0 + 2) + (char)(iVar6 % iVar8);
  *(undefined1 *)(DAT_800dcbd0 + 0x20) =
       *(undefined1 *)(*(int *)(DAT_800dcbd0 + 0x70) + (uint)*(byte *)(DAT_800dcbd0 + 0x21) * 8 + 2)
  ;
  *(char *)(DAT_800dcbd0 + 0x1f) = *(char *)(DAT_800dcbd0 + 0x1f) + *(char *)(DAT_800dcbd0 + 0xb);
  uVar5 = DAT_800dcbd0;
  uVar7 = (uint)*(byte *)(DAT_800dcbd0 + 0x1f) * 0x18 + *(uint *)(DAT_800dcbd0 + 0x78);
  uVar3 = uVar7 + 3 & 3;
  uVar12 = uVar7 & 3;
  uVar9 = (*(int *)((uVar7 + 3) - uVar3) << (3 - uVar3) * 8 |
          *(uint *)(DAT_800dcbd0 + 0x78) & 0xffffffffU >> (uVar3 + 1) * 8) & -1 << (4 - uVar12) * 8
          | *(uint *)(uVar7 - uVar12) >> uVar12 * 8;
  uVar3 = uVar7 + 7 & 3;
  uVar12 = uVar7 + 4 & 3;
  uVar11 = (*(int *)((uVar7 + 7) - uVar3) << (3 - uVar3) * 8 |
           uVar10 & 0xffffffffU >> (uVar3 + 1) * 8) & -1 << (4 - uVar12) * 8 |
           *(uint *)((uVar7 + 4) - uVar12) >> uVar12 * 8;
  uVar3 = uVar7 + 0xb & 3;
  uVar10 = uVar7 + 8 & 3;
  uVar12 = (*(int *)((uVar7 + 0xb) - uVar3) << (3 - uVar3) * 8 |
           param_3 & 0xffffffffU >> (uVar3 + 1) * 8) & -1 << (4 - uVar10) * 8 |
           *(uint *)((uVar7 + 8) - uVar10) >> uVar10 * 8;
  uVar3 = uVar7 + 0xf & 3;
  uVar10 = uVar7 + 0xc & 3;
  uVar10 = (*(int *)((uVar7 + 0xf) - uVar3) << (3 - uVar3) * 8 |
           param_4 & 0xffffffffU >> (uVar3 + 1) * 8) & -1 << (4 - uVar10) * 8 |
           *(uint *)((uVar7 + 0xc) - uVar10) >> uVar10 * 8;
  uVar3 = DAT_800dcbd0 + 3 & 3;
  puVar4 = (uint *)((DAT_800dcbd0 + 3) - uVar3);
  *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar9 >> (3 - uVar3) * 8;
  uVar3 = uVar5 & 3;
  *(uint *)(uVar5 - uVar3) =
       *(uint *)(uVar5 - uVar3) & 0xffffffffU >> (4 - uVar3) * 8 | uVar9 << uVar3 * 8;
  uVar3 = uVar5 + 7 & 3;
  puVar4 = (uint *)((uVar5 + 7) - uVar3);
  *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar11 >> (3 - uVar3) * 8;
  uVar3 = uVar5 + 4 & 3;
  puVar4 = (uint *)((uVar5 + 4) - uVar3);
  *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar3) * 8 | uVar11 << uVar3 * 8;
  uVar3 = uVar5 + 0xb & 3;
  puVar4 = (uint *)((uVar5 + 0xb) - uVar3);
  *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar12 >> (3 - uVar3) * 8;
  uVar3 = uVar5 + 8 & 3;
  puVar4 = (uint *)((uVar5 + 8) - uVar3);
  *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar3) * 8 | uVar12 << uVar3 * 8;
  uVar3 = uVar5 + 0xf & 3;
  puVar4 = (uint *)((uVar5 + 0xf) - uVar3);
  *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar10 >> (3 - uVar3) * 8;
  uVar3 = uVar5 + 0xc & 3;
  puVar4 = (uint *)((uVar5 + 0xc) - uVar3);
  *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar3) * 8 | uVar10 << uVar3 * 8;
  uVar3 = uVar7 + 0x13 & 3;
  uVar10 = uVar7 + 0x10 & 3;
  uVar12 = (*(int *)((uVar7 + 0x13) - uVar3) << (3 - uVar3) * 8 |
           uVar9 & 0xffffffffU >> (uVar3 + 1) * 8) & -1 << (4 - uVar10) * 8 |
           *(uint *)((uVar7 + 0x10) - uVar10) >> uVar10 * 8;
  uVar3 = uVar7 + 0x17 & 3;
  uVar10 = uVar7 + 0x14 & 3;
  uVar10 = (*(int *)((uVar7 + 0x17) - uVar3) << (3 - uVar3) * 8 |
           uVar11 & 0xffffffffU >> (uVar3 + 1) * 8) & -1 << (4 - uVar10) * 8 |
           *(uint *)((uVar7 + 0x14) - uVar10) >> uVar10 * 8;
  uVar3 = uVar5 + 0x13 & 3;
  puVar4 = (uint *)((uVar5 + 0x13) - uVar3);
  *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar12 >> (3 - uVar3) * 8;
  uVar3 = uVar5 + 0x10 & 3;
  puVar4 = (uint *)((uVar5 + 0x10) - uVar3);
  *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar3) * 8 | uVar12 << uVar3 * 8;
  uVar3 = uVar5 + 0x17 & 3;
  puVar4 = (uint *)((uVar5 + 0x17) - uVar3);
  *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar10 >> (3 - uVar3) * 8;
  uVar3 = uVar5 + 0x14 & 3;
  puVar4 = (uint *)((uVar5 + 0x14) - uVar3);
  *puVar4 = *puVar4 & 0xffffffffU >> (4 - uVar3) * 8 | uVar10 << uVar3 * 8;
  return;
}


