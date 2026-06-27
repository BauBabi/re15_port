/* FUN_80074bb0 @ 0x80074bb0  (Ghidra headless, raw MIPS overlay) */

void FUN_80074bb0(void)

{
  byte bVar1;
  byte bVar2;
  short sVar3;
  short sVar4;
  u_short uVar5;
  short *psVar6;
  int iVar7;
  u_short *puVar8;
  int iVar9;
  uint uVar10;
  undefined *puVar11;
  undefined *puVar12;
  undefined *ot;
  
  sVar4 = DAT_800d5c26;
  sVar3 = DAT_800d5c24;
  bVar2 = DAT_800d46ac;
  puVar11 = &UNK_8019ac00;
  uVar10 = 7;
  DAT_800ab11e = (DAT_800d46ac - 6) * 0xf + 0x5d;
  ot = &UNK_800cc1fc + (uint)DAT_800ce5e0 * 0x20;
  if (DAT_800ce5e0 != 0) {
    puVar11 = &DAT_8019ac14;
  }
  psVar6 = (short *)(puVar11 + 10);
  bVar1 = DAT_800d46ac >> 1;
  iVar7 = 0x1c;
  do {
    puVar12 = puVar11;
    uVar10 = uVar10 - 1;
    psVar6[3] = 5;
    psVar6[4] = (ushort)bVar1 * 0x1e;
    psVar6[-1] = *(short *)(&DAT_800ab108 + iVar7) + sVar3;
    *psVar6 = *(short *)(&DAT_800ab10a + iVar7) + sVar4;
    AddPrim(ot,puVar12);
    psVar6 = psVar6 + 0x14;
    iVar7 = iVar7 + -4;
    puVar11 = puVar12 + 0x28;
  } while (5 < uVar10);
  psVar6 = (short *)(puVar12 + 0x32);
  iVar7 = uVar10 * 4;
  do {
    puVar12 = puVar11;
    psVar6[-1] = *(short *)(&DAT_800ab108 + iVar7) + sVar3;
    *psVar6 = *(short *)(&DAT_800ab10a + iVar7) + sVar4;
    uVar10 = uVar10 - 1;
    if ((DAT_800d5c00 != '\x01') || (uVar10 != 2)) {
      AddPrim(ot,puVar12);
    }
    psVar6 = psVar6 + 0x14;
    puVar11 = puVar12 + 0x28;
    iVar7 = iVar7 + -4;
  } while (2 < uVar10);
  puVar8 = (u_short *)(puVar12 + 0x36);
  AddPrim(ot,&UNK_800d6a28 + (uint)DAT_800ce5e0 * 0xc);
  iVar7 = uVar10 * 4;
  do {
    iVar9 = iVar7 + -4;
    puVar8[-3] = *(short *)(&DAT_800ab108 + iVar7) + sVar3;
    puVar8[-2] = *(short *)(&DAT_800ab10a + iVar7) + sVar4;
    if (iVar9 == 0) {
      if (DAT_800d4a64 == 'N') {
        *(undefined1 *)(puVar8 + -1) = 0x50;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
        *(undefined1 *)((int)puVar8 + -1) = 0x5a;
      }
      if (DAT_800d4a64 == 'R') {
        *(undefined1 *)(puVar8 + -1) = 0x50;
                    /* Possible PsyQ macro: setTile8() */
        *(undefined1 *)((int)puVar8 + -1) = 0x78;
      }
    }
    else {
      puVar8[1] = 0x50;
      puVar8[2] = (ushort)(bVar2 >> 1) * 0x1e;
    }
    uVar5 = GetClut(0,0x1e5);
    *puVar8 = uVar5;
    AddPrim(ot,puVar11);
    puVar8 = puVar8 + 0x14;
    puVar11 = puVar11 + 0x28;
    iVar7 = iVar9;
  } while (iVar9 != 0);
  AddPrim(ot,&UNK_800d6b48 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


