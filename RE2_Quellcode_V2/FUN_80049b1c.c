/* FUN_80049b1c @ 0x80049b1c  (Ghidra headless, raw MIPS overlay) */

void FUN_80049b1c(void)

{
  ushort *puVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  ushort *puVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  ushort *puVar11;
  undefined1 *puVar12;
  ushort *puVar13;
  ushort *puVar14;
  ushort *puVar15;
  
  puVar14 = *(ushort **)(DAT_800d4820 * 0x20 + *(int *)(DAT_800ce324 + 0x24) + 0x1c);
  if (*(int *)puVar14 == -1) {
    *DAT_800ce324 = 0;
  }
  else {
    *DAT_800ce324 = (byte)((uint)*(int *)puVar14 >> 0x10);
    uVar8 = (uint)*DAT_800ce324;
    uVar9 = uVar8;
    puVar12 = DAT_800eaad0;
    do {
      *puVar12 = 0;
      uVar9 = uVar9 - 1;
      puVar12 = puVar12 + 4;
    } while (uVar9 != 0);
    uVar9 = (uint)*puVar14;
    puVar15 = puVar14 + 2;
    puVar11 = puVar15 + uVar9 * 4;
    if (uVar9 != 0) {
      puVar14 = puVar14 + 5;
      do {
        uVar10 = (uint)*puVar15;
        puVar7 = puVar11 + 4;
        puVar13 = (ushort *)(puVar12 + 2);
        do {
          uVar10 = uVar10 - 1;
          puVar12 = puVar12 + -4;
          *puVar12 = 1;
          *(char *)((int)puVar13 + -5) = (char)uVar9;
          uVar8 = uVar8 - 1;
          puVar13[-2] = puVar7[-2];
          uVar2 = *puVar11;
          uVar3 = puVar7[-1];
          iVar5 = (uint)(byte)puVar7[-3] + (int)(short)puVar14[-1] +
                  ((uint)*(byte *)((int)puVar7 + -5) + (int)(short)*puVar14) * 0x10000;
          if (uVar3 == 0) {
            puVar1 = puVar7 + 1;
            uVar3 = *puVar7;
            puVar7 = puVar7 + 6;
            puVar11 = puVar11 + 6;
            iVar6 = (uint)*puVar1 * 0x10000 + (uint)uVar3;
          }
          else {
            iVar6 = CONCAT22(uVar3,uVar3);
            puVar7 = puVar7 + 4;
            puVar11 = puVar11 + 4;
          }
          iVar4 = DAT_800ea240 + uVar8 * 0x20;
          *(undefined4 *)(iVar4 + 0x10) = 0x65808080;
          *(int *)(iVar4 + 0x14) = iVar5;
          *(uint *)(iVar4 + 0x18) = uVar2 + 0x78000000;
          *(int *)(iVar4 + 0x1c) = iVar6;
          iVar4 = DAT_800ea244 + uVar8 * 0x20;
          *(undefined4 *)(iVar4 + 0x10) = 0x65808080;
          *(int *)(iVar4 + 0x14) = iVar5;
          *(uint *)(iVar4 + 0x18) = uVar2 + 0x78000000;
          *(int *)(iVar4 + 0x1c) = iVar6;
          puVar13 = puVar13 + -2;
        } while (uVar10 != 0);
        puVar14 = puVar14 + 4;
        uVar9 = uVar9 - 1;
        puVar15 = puVar15 + 4;
      } while (uVar9 != 0);
    }
  }
  return;
}


