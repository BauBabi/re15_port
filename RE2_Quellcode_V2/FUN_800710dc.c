/* FUN_800710dc @ 0x800710dc  (Ghidra headless, raw MIPS overlay) */

void FUN_800710dc(void)

{
  ushort uVar1;
  short sVar2;
  short sVar3;
  undefined1 uVar4;
  byte bVar5;
  u_short uVar6;
  undefined *ot;
  short sVar7;
  int iVar8;
  uint *puVar9;
  int iVar10;
  uint uVar11;
  char cVar12;
  short sVar13;
  int iVar14;
  undefined *puVar15;
  short *psVar16;
  char cVar17;
  byte *pbVar18;
  char cVar19;
  undefined *puVar20;
  uint *puVar21;
  undefined *puVar22;
  short sVar23;
  short sVar24;
  int iVar25;
  undefined *local_70;
  short local_50;
  int local_48;
  short local_40;
  
  sVar3 = DAT_800d5c42;
  sVar2 = DAT_800d5c40;
  puVar21 = &DAT_80198000;
  puVar22 = &DAT_80198800;
  puVar20 = &DAT_80199200;
  local_70 = &DAT_80199000;
  iVar25 = (int)DAT_800d5c42;
  iVar8 = (uint)DAT_800ce5e0 * 0x20;
  ot = &UNK_800cc1f0 + iVar8;
  iVar14 = (int)DAT_800d5c40;
  puVar9 = (uint *)(&DAT_800cc200 + iVar8);
  if (DAT_800ce5e0 != 0) {
    puVar20 = &DAT_80199214;
    puVar21 = &DAT_80198028;
    local_70 = &DAT_80199010;
    puVar22 = &DAT_80198818;
  }
  if (DAT_800d766c == 0) {
    DAT_800d4cd0 = DAT_800d4cd0 + 2;
  }
  else {
    DAT_800d4cd0 = DAT_800d4cd0 + -2;
  }
  if (0x3e < DAT_800d4cd0) {
    DAT_800d4cd0 = 0x3f;
    DAT_800d766c = 1;
  }
  if (DAT_800d4cd0 < 1) {
    DAT_800d4cd0 = 0;
    DAT_800d766c = 0;
  }
  puVar15 = puVar22 + 6;
  pbVar18 = &UNK_800a9c38;
  sVar24 = DAT_800d5c40 + 6;
  sVar23 = DAT_800d5c40 + 0xc5;
  iVar8 = 0x10;
  do {
    pbVar18 = pbVar18 + -4;
    *(short *)(puVar15 + 2) = sVar24;
    bVar5 = *pbVar18;
    *(short *)(puVar15 + 6) = sVar23;
    *(ushort *)(puVar15 + 4) = (ushort)bVar5 + sVar3;
    bVar5 = *pbVar18;
    *(short *)(puVar15 + 10) = sVar24;
    *(ushort *)(puVar15 + 8) = (ushort)bVar5 + sVar3;
    bVar5 = *pbVar18;
    *(short *)(puVar15 + 0xe) = sVar23;
    *(ushort *)(puVar15 + 0xc) = (ushort)bVar5 + sVar3 + 0x14;
    *(ushort *)(puVar15 + 0x10) = (ushort)*pbVar18 + sVar3 + 0x14;
    puVar15[-2] = (&DAT_800a9c25)[iVar8];
    puVar15[-1] = (&DAT_800a9c26)[iVar8];
    *puVar15 = (&DAT_800a9c27)[iVar8];
    AddPrim(ot,puVar22);
    puVar15 = puVar15 + 0x30;
    iVar8 = iVar8 + -4;
    puVar22 = puVar22 + 0x30;
  } while (pbVar18 != &UNK_800a9c28);
  *(short *)(puVar20 + 8) = DAT_800a9c98 + sVar2;
  *(short *)(puVar20 + 10) = DAT_800a9c9a + sVar3;
  uVar6 = GetClut(0x100,0x1f0);
  *(u_short *)(puVar20 + 0xe) = uVar6;
  uVar4 = 0x40;
  if (DAT_800d5bf1 == '\x02') {
    uVar4 = 0x80;
  }
  puVar20[4] = uVar4;
  puVar20[5] = uVar4;
  puVar20[6] = uVar4;
  AddPrim(ot,puVar20);
  AddPrim(ot,&UNK_800d6b00 + (uint)DAT_800ce5e0 * 0xc);
  *(short *)(puVar20 + 0x30) = DAT_800a9c94 + sVar2;
  *(short *)(puVar20 + 0x32) = DAT_800a9c96 + sVar3;
  uVar6 = GetClut(0,0x1e4);
  *(u_short *)(puVar20 + 0x36) = uVar6;
  AddPrim(ot,puVar20 + 0x28);
  AddPrim(ot,&UNK_800d6a10 + (uint)DAT_800ce5e0 * 0xc);
  *(short *)(puVar20 + 0x58) = DAT_800a9c90 + sVar2;
  *(short *)(puVar20 + 0x5a) = DAT_800a9c92 + sVar3;
  if (((DAT_800ce304 & 0x4008) == 0) || (DAT_800d5bf2 == 0)) {
    puVar20[0x5c] = 0x12;
    uVar4 = 0x78;
  }
  else {
    puVar20[0x5c] = 0x12;
    uVar4 = 100;
  }
  puVar20[0x5d] = uVar4;
  AddPrim(ot,puVar20 + 0x50);
  *(short *)(puVar20 + 0x80) = DAT_800a9c8c + sVar2;
  *(short *)(puVar20 + 0x82) = DAT_800a9c8e + sVar3;
  if (((DAT_800ce304 & 0x1004) == 0) || (DAT_800d5bf2 == 0)) {
    puVar20[0x84] = 0x12;
    uVar4 = 0x50;
  }
  else {
    puVar20[0x84] = 0x12;
    uVar4 = 0x3c;
  }
  puVar20[0x85] = uVar4;
  AddPrim(ot,puVar20 + 0x78);
  iVar8 = 5;
  psVar16 = (short *)(puVar20 + 0xaa);
  puVar22 = puVar20 + 0xa0;
  do {
    puVar20 = puVar22;
    iVar8 = iVar8 + -1;
    iVar10 = (0x14 - iVar8) * 4;
    psVar16[-1] = *(short *)(&UNK_800a9c38 + iVar10) + sVar2;
    *psVar16 = *(short *)(&UNK_800a9c3a + iVar10) + sVar3 +
               ((ushort)DAT_800d5c14 + (short)iVar8 & 0x3f) * 2;
    AddPrim(ot,puVar20);
    psVar16 = psVar16 + 0x14;
    puVar22 = puVar20 + 0x28;
  } while (iVar8 != 0);
  psVar16 = (short *)(puVar20 + 0x32);
  iVar8 = 0x40;
  do {
    iVar10 = iVar8 + -4;
    psVar16[-1] = *(short *)(&DAT_800a9c34 + iVar8) + sVar2;
    *psVar16 = *(short *)(&DAT_800a9c36 + iVar8) + sVar3;
    AddPrim(ot,puVar22);
    psVar16 = psVar16 + 0x14;
    iVar8 = iVar10;
    puVar22 = puVar22 + 0x28;
  } while (iVar10 != 0);
  AddPrim(ot,&DAT_800d6bd8 + (uint)DAT_800ce5e0 * 0xc);
  uVar11 = (uint)DAT_800d5bf2;
  if (uVar11 - 2 < 2) {
    if (uVar11 == 2) {
      local_50 = -0x14;
      local_48 = -1;
    }
    if (uVar11 == 3) {
      local_50 = 100;
      local_48 = 5;
    }
    sVar23 = 0x30;
    if ((&DAT_800d4a6a)[((uint)DAT_800d5c14 + local_48 & 0x3f) * 4] == '\0') {
      sVar23 = 0x18;
      uVar4 = 0x50;
    }
    else {
      uVar4 = 0x78;
    }
                    /* Possible PsyQ macro: setPolyGT4() */
    *(undefined1 *)((int)puVar21 + 0xd) = 0x3c;
                    /* Possible PsyQ macro: setPolyGT4() */
    *(undefined1 *)((int)puVar21 + 0x15) = 0x3c;
    *(undefined1 *)(puVar21 + 3) = 0x28;
    *(undefined1 *)(puVar21 + 5) = uVar4;
    *(undefined1 *)(puVar21 + 7) = 0x28;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
    *(undefined1 *)((int)puVar21 + 0x1d) = 0x5a;
    *(undefined1 *)(puVar21 + 9) = uVar4;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
    *(undefined1 *)((int)puVar21 + 0x25) = 0x5a;
    *(short *)(puVar21 + 2) = DAT_800d5c40 + 0x92;
    *(short *)((int)puVar21 + 10) = (short)DAT_800d5c15 + DAT_800d5c42 + local_50 + 0x1b;
    *(short *)(puVar21 + 4) = DAT_800d5c40 + sVar23 + 0x93;
    *(short *)((int)puVar21 + 0x12) = (short)DAT_800d5c15 + DAT_800d5c42 + local_50 + 0x1b;
    *(short *)(puVar21 + 6) = DAT_800d5c40 + 0x92;
    *(short *)((int)puVar21 + 0x1a) = (short)DAT_800d5c15 + DAT_800d5c42 + local_50 + 0x2e;
    *(short *)(puVar21 + 8) = DAT_800d5c40 + sVar23 + 0x93;
    *(short *)((int)puVar21 + 0x22) = (short)DAT_800d5c15 + DAT_800d5c42 + local_50 + 0x2e;
                    /* Probable PsyQ macro: addPrim(). */
    *puVar21 = *puVar21 & 0xff000000 | *puVar9 & 0xffffff;
    *puVar9 = *puVar9 & 0xff000000 | (uint)puVar21 & 0xffffff;
  }
  iVar8 = 5;
  psVar16 = (short *)((int)puVar21 + 0x72);
  sVar23 = 0x7f;
  iVar10 = 100;
  cVar19 = '\x0e';
  cVar17 = -0x10;
  local_40 = sVar2 + 0x92;
  sVar24 = 0x7e;
  do {
    puVar21 = puVar21 + 0x14;
    sVar23 = sVar23 + -0x14;
    iVar10 = iVar10 + -0x14;
    cVar19 = cVar19 + -0x1e;
    iVar8 = iVar8 + -1;
    cVar17 = cVar17 + -0x1e;
    if ((&DAT_800d4a6a)[((uint)DAT_800d5c14 + iVar8 & 0x3f) * 4] == '\0') {
      sVar13 = 0x18;
      uVar4 = 0x50;
    }
    else {
      sVar13 = 0x30;
      uVar4 = 0x78;
    }
    *(undefined1 *)(psVar16 + -7) = uVar4;
    *(undefined1 *)(psVar16 + -0xb) = 0x28;
    *(char *)((int)psVar16 + -0x15) = cVar17;
    *(char *)((int)psVar16 + -0xd) = cVar17;
    *(undefined1 *)(psVar16 + -3) = 0x28;
    *(char *)((int)psVar16 + -5) = cVar19;
    *(undefined1 *)(psVar16 + 1) = uVar4;
    *(char *)((int)psVar16 + 3) = cVar19;
    psVar16[-0xd] = local_40;
    sVar7 = sVar2 + sVar13 + 0x93;
    sVar13 = (short)DAT_800d5c15;
    psVar16[-9] = sVar7;
    psVar16[-0xc] = sVar13 + sVar3 + sVar23;
    sVar13 = (short)DAT_800d5c15;
    psVar16[-5] = local_40;
    psVar16[-8] = sVar13 + sVar3 + sVar23;
    cVar12 = DAT_800d5c15;
    psVar16[-1] = sVar7;
    psVar16[-4] = (short)cVar12 + sVar3 + sVar24;
    *psVar16 = (short)DAT_800d5c15 + sVar3 + sVar24;
                    /* Probable PsyQ macro: addPrim(). */
    *puVar21 = *puVar21 & 0xff000000 | *puVar9 & 0xffffff;
    psVar16 = psVar16 + 0x28;
    *puVar9 = *puVar9 & 0xff000000 | (uint)puVar21 & 0xffffff;
    cVar12 = (&DAT_800d4a68)[(iVar8 + (uint)DAT_800d5c14 & 0x3f) * 4];
    if (cVar12 == '\0') {
      cVar12 = 'd';
    }
    FUN_800693d0(iVar14 + 7,iVar10 + iVar25 + 0x1d + (int)DAT_800d5c15,6,cVar12);
    sVar24 = sVar24 + -0x14;
  } while (iVar8 != 0);
  *(short *)(local_70 + 8) = sVar2 + 6;
  cVar17 = DAT_800d5c15;
  uVar1 = (ushort)DAT_800d5c14;
  *(short *)(local_70 + 0xc) = sVar2 + 0xc4;
  *(ushort *)(local_70 + 10) = (short)cVar17 + sVar3 + (0x3f - (uVar1 - 4 & 0x3f)) * 0x14 + 7;
  *(ushort *)(local_70 + 0xe) =
       (short)DAT_800d5c15 + sVar3 + (0x3f - (DAT_800d5c14 - 4 & 0x3f)) * 0x14 + 7;
  bVar5 = DAT_800d5c14 + 2 & 0x3f;
  if ((bVar5 < 5) || (bVar5 == 0x3f)) {
    AddPrim(ot,local_70);
  }
  iVar8 = 2;
  iVar25 = iVar25 + 0x6a;
  pbVar18 = local_70 + 0x26;
  do {
    local_70 = local_70 + 0x20;
    iVar25 = iVar25 + -0x14;
    *(short *)(pbVar18 + 4) = (short)iVar25;
    *(short *)(pbVar18 + 8) = (short)iVar25;
    *(short *)(pbVar18 + 2) = sVar2 + 6;
    *(short *)(pbVar18 + 6) = sVar2 + 0xc4;
    iVar8 = iVar8 + -1;
    if (DAT_800d5bf2 == 0) {
      pbVar18[-2] = 0x30;
      pbVar18[-1] = 0x20;
      bVar5 = 8;
    }
    else {
      pbVar18[-2] = (byte)DAT_800d4cd0 + 0x50;
      pbVar18[-1] = (byte)DAT_800d4cd0 + 0x40;
      bVar5 = (byte)DAT_800d4cd0 & 0x1f;
    }
    *pbVar18 = bVar5;
    pbVar18 = pbVar18 + 0x20;
    AddPrim(ot,local_70);
  } while (iVar8 != 0);
  return;
}


