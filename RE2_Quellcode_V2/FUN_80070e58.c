/* FUN_80070e58 @ 0x80070e58  (Ghidra headless, raw MIPS overlay) */

void FUN_80070e58(void)

{
  bool bVar1;
  byte bVar2;
  u_short uVar3;
  u_short *puVar4;
  byte *pbVar5;
  int iVar6;
  undefined *puVar7;
  u_char *puVar8;
  int iVar9;
  u_char uVar10;
  u_char uVar11;
  POLY_FT4 *p;
  POLY_F4 *p_00;
  
  p = (POLY_FT4 *)&DAT_80198000;
  p_00 = (POLY_F4 *)&DAT_80198800;
  puVar7 = &DAT_80199200;
  uVar3 = GetClut(0,0x1e7);
  iVar6 = 0x60;
  do {
    iVar9 = 2;
    puVar4 = (u_short *)(puVar7 + 0xe);
    do {
      *(undefined1 *)((int)puVar4 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar4 + -7) = 100;
      *(undefined1 *)(puVar4 + -5) = 0x80;
      *(undefined1 *)((int)puVar4 + -9) = 0x80;
      *(undefined1 *)(puVar4 + -4) = 0x80;
      *(undefined *)(puVar4 + -1) = (&UNK_800a9bc4)[iVar6];
      *(undefined *)((int)puVar4 + -1) = (&UNK_800a9bc5)[iVar6];
      puVar7 = puVar7 + 0x14;
      puVar4[1] = (ushort)(byte)(&UNK_800a9bc6)[iVar6];
      bVar2 = (&UNK_800a9bc7)[iVar6];
      iVar9 = iVar9 + -1;
      *puVar4 = uVar3;
      puVar4[2] = (ushort)bVar2;
      puVar4 = puVar4 + 10;
    } while (iVar9 != 0);
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -4;
  } while (bVar1);
  iVar6 = 6;
  uVar10 = '\x0e';
  uVar11 = '\x0e';
  do {
    uVar10 = uVar10 + 0xe2;
    iVar6 = iVar6 + -1;
    iVar9 = 2;
    puVar8 = &p->b0;
    do {
      SetPolyFT4(p);
      puVar8[6] = '(';
      puVar8[7] = uVar10;
      puVar8[0xe] = 'P';
      puVar8[0xf] = uVar10;
      puVar8[0x16] = '(';
      puVar8[0x17] = uVar11;
      puVar8[0x1e] = 'P';
      puVar8[0x1f] = uVar11;
      uVar3 = GetTPage(1,0,0x1c0,0x100);
      *(u_short *)(puVar8 + 0x10) = uVar3;
      uVar3 = GetClut(0,0x1e5);
      p = p + 1;
      iVar9 = iVar9 + -1;
      *(u_short *)(puVar8 + 8) = uVar3;
      puVar8[-2] = 0x80;
      puVar8[-1] = 0x80;
      *puVar8 = 0x80;
      puVar8[1] = puVar8[1] & 0xfd;
      puVar8 = puVar8 + 0x28;
    } while (iVar9 != 0);
    uVar11 = uVar11 + 0xe2;
  } while (iVar6 != 0);
  iVar6 = 3;
  do {
    iVar9 = 2;
    puVar8 = &p_00->b0;
    do {
      SetPolyF4(p_00);
      p_00 = p_00 + 1;
      iVar9 = iVar9 + -1;
                    /* Possible PsyQ macro: setPolyF3() */
      puVar8[-2] = ' ';
      puVar8[-1] = ' ';
      *puVar8 = '@';
      puVar8[1] = puVar8[1] | 2;
      puVar8 = puVar8 + 0x18;
    } while (iVar9 != 0);
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -1;
  } while (bVar1);
  iVar6 = 6;
  pbVar5 = &DAT_80199007;
  do {
                    /* Possible PsyQ macro: setLineF2() */
    *pbVar5 = 0x40;
    iVar6 = iVar6 + -1;
    pbVar5[-4] = 3;
    pbVar5[-3] = 0x80;
    pbVar5[-2] = 0x20;
    pbVar5[-1] = 0x20;
    *pbVar5 = *pbVar5 & 0xfd;
    pbVar5 = pbVar5 + 0x10;
  } while (iVar6 != 0);
  DAT_800d5c40 = 7;
  DAT_800d5c42 = 0xe;
  return;
}


