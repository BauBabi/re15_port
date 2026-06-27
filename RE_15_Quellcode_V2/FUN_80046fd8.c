undefined * FUN_80046fd8(void)

{
  ushort uVar1;
  u_short uVar2;
  short sVar3;
  undefined2 uVar4;
  u_short uVar5;
  uint uVar6;
  u_short *puVar7;
  undefined1 *puVar8;
  u_short *puVar9;
  ushort uVar10;
  int iVar11;
  undefined *puVar12;
  undefined *puVar13;
  u_short *puVar14;
  
  uVar5 = GetClut(0x100,0x1f5);
  iVar11 = (uint)DAT_800b260d * 8;
  uVar6 = (uint)*(ushort *)(&DAT_800768b4 + iVar11);
  if (uVar6 == 0) {
    trap(0x1c00);
  }
  if ((uVar6 == 0xffffffff) && (DAT_800aca88 + 25000 == -0x80000000)) {
    trap(0x1800);
  }
  puVar14 = (u_short *)(&PTR_u_x<8HH0LLH0_80076844)[(uint)DAT_800b260e * 2];
  uVar1 = *(ushort *)(&DAT_80076840 + (uint)DAT_800b260e * 8);
  DAT_800b2604 = (short)((DAT_800aca88 + 25000) / (int)uVar6) + *(short *)(&DAT_800768b0 + iVar11);
  uVar6 = (uint)*(ushort *)(&DAT_800768b6 + iVar11);
  if (uVar6 == 0) {
    trap(0x1c00);
  }
  if ((uVar6 == 0xffffffff) && (-(DAT_800aca90 + 25000) == -0x80000000)) {
    trap(0x1800);
  }
  uVar10 = 0;
  puVar8 = &DAT_8019a825;
  DAT_800b2606 = (short)(-(DAT_800aca90 + 25000) / (int)uVar6) + *(short *)(&DAT_800768b2 + iVar11);
  do {
    puVar8[-0x22] = 9;
    puVar8[-0x1e] = 0x2e;
    puVar8[-0x21] = 0x80;
    puVar8[-0x20] = 0x80;
    puVar8[-0x1f] = 0x80;
    uVar4 = DAT_800b261c;
    *(undefined2 *)(puVar8 + -0xf) = 0x1b;
    *(undefined2 *)(puVar8 + -0x17) = uVar4;
    *(short *)(puVar8 + -0x1d) = DAT_800b2604 + -4;
    *(short *)(puVar8 + -0x1b) = DAT_800b2606 + -4;
    *(short *)(puVar8 + -0x15) = DAT_800b2604 + 4;
    *(short *)(puVar8 + -0x13) = DAT_800b2606 + -4;
    *(short *)(puVar8 + -0xd) = DAT_800b2604 + -4;
    *(short *)(puVar8 + -0xb) = DAT_800b2606 + 4;
    *(short *)(puVar8 + -5) = DAT_800b2604 + 4;
    sVar3 = DAT_800b2606;
    uVar10 = uVar10 + 1;
    puVar8[-0x19] = 0xe0;
    puVar8[-0x18] = 0x80;
    puVar8[-0x11] = 0xe8;
    puVar8[-0x10] = 0x80;
    puVar8[-9] = 0xe0;
    puVar8[-8] = 0x88;
    puVar8[-1] = 0xe8;
    *puVar8 = 0x88;
    *(short *)(puVar8 + -3) = sVar3 + 4;
    puVar8 = puVar8 + 0x28;
  } while (uVar10 < 2);
  uVar10 = 0;
  puVar7 = &DAT_8019a90e;
  puVar12 = &DAT_8019a900;
  do {
    puVar13 = puVar12;
    *(undefined1 *)((int)puVar7 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar7 + -7) = 0x66;
    *(undefined1 *)(puVar7 + -5) = 0x80;
    *(undefined1 *)((int)puVar7 + -9) = 0x80;
    *(undefined1 *)(puVar7 + -4) = 0x80;
    puVar7[-3] = 0x1e;
    puVar7[-2] = 0x1e;
    puVar7[1] = 0x58;
    puVar7[2] = 0x20;
    *(undefined1 *)(puVar7 + -1) = 0;
    *(undefined1 *)((int)puVar7 + -1) = 0;
    *puVar7 = uVar5;
    puVar7 = puVar7 + 10;
    uVar10 = uVar10 + 1;
    puVar12 = puVar13 + 0x14;
  } while (uVar10 < 2);
  uVar10 = 0;
  puVar7 = (u_short *)(puVar13 + 0x22);
  do {
    puVar13 = puVar12;
    *(undefined1 *)((int)puVar7 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar7 + -7) = 0x66;
    *(undefined1 *)(puVar7 + -5) = 0x80;
    *(undefined1 *)((int)puVar7 + -9) = 0x80;
    *(undefined1 *)(puVar7 + -4) = 0x80;
    puVar7[-3] = 0x10e;
    puVar7[-2] = 0x28;
    puVar7[1] = 0x20;
    puVar7[2] = 0x30;
    *(undefined1 *)(puVar7 + -1) = 0x60;
    *(undefined1 *)((int)puVar7 + -1) = 0;
    *puVar7 = uVar5;
    puVar7 = puVar7 + 10;
    uVar10 = uVar10 + 1;
    puVar12 = puVar13 + 0x14;
  } while (uVar10 < 2);
  uVar10 = 0;
  if (uVar1 != 0) {
    puVar9 = puVar14 + 1;
    puVar7 = (u_short *)(puVar13 + 0x22);
    do {
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar7 + -7) = 100;
      *(undefined1 *)((int)puVar7 + -0xb) = 4;
      *(undefined1 *)(puVar7 + -5) = 0x80;
      *(undefined1 *)((int)puVar7 + -9) = 0x80;
      *(undefined1 *)(puVar7 + -4) = 0x80;
      *(byte *)((int)puVar7 + -7) = *(byte *)((int)puVar7 + -7) | 2;
      puVar7[1] = puVar9[1];
      puVar7[2] = puVar9[2];
      *(char *)(puVar7 + -1) = (char)puVar9[3];
      *(char *)((int)puVar7 + -1) = (char)puVar9[4];
      puVar7[-3] = *puVar14;
      uVar2 = *puVar9;
      *puVar7 = uVar5;
      puVar7[-2] = uVar2;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar7 + 0xd) = 100;
      *(undefined1 *)((int)puVar7 + 9) = 4;
      *(undefined1 *)(puVar7 + 5) = 0x80;
      *(undefined1 *)((int)puVar7 + 0xb) = 0x80;
      *(undefined1 *)(puVar7 + 6) = 0x80;
      *(byte *)((int)puVar7 + 0xd) = *(byte *)((int)puVar7 + 0xd) | 2;
      puVar7[0xb] = puVar9[1];
      puVar7[0xc] = puVar9[2];
      puVar12 = puVar12 + 0x28;
      *(char *)(puVar7 + 9) = (char)puVar9[3];
      uVar10 = uVar10 + 1;
      *(char *)((int)puVar7 + 0x13) = (char)puVar9[4];
      uVar2 = *puVar14;
      puVar14 = puVar14 + 6;
      puVar7[7] = uVar2;
      uVar2 = *puVar9;
      puVar9 = puVar9 + 6;
      puVar7[10] = uVar5;
      puVar7[8] = uVar2;
      puVar7 = puVar7 + 0x14;
    } while (uVar10 < uVar1);
  }
  return puVar12;
}
