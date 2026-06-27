/* FUN_8006e120 @ 0x8006e120  (Ghidra headless, raw MIPS overlay) */

void FUN_8006e120(void)

{
  byte bVar1;
  undefined1 uVar2;
  u_short uVar3;
  uint uVar4;
  uint uVar5;
  undefined *puVar6;
  int iVar7;
  undefined1 uVar8;
  short *psVar9;
  u_short *puVar10;
  short sVar11;
  undefined1 *puVar12;
  char cVar14;
  int iVar15;
  uint uVar16;
  short sVar17;
  undefined *puVar18;
  undefined *p;
  int unaff_s5;
  undefined *ot;
  undefined1 *puVar13;
  
  puVar18 = &UNK_8019d000;
  ot = &UNK_800cc1fc + (uint)DAT_800ce5e0 * 0x20;
  uVar4 = FUN_8006e7f0((int)DAT_800d481c,(int)DAT_800d481e);
  if (DAT_800ce5e0 != 0) {
    puVar18 = &DAT_8019d014;
  }
  uVar5 = FUN_8006eae8(DAT_800d5c0a,(int)DAT_800d481e);
  puVar6 = (&PTR_DAT_800aaa38)[(uint)DAT_800d5c0a * 2];
  if (DAT_800d5c0a == uVar4) {
    DAT_800d5c44 = (short)((DAT_800cfc30 + 28000) / 0x1c2) + DAT_800d5c48;
    DAT_800d5c46 = DAT_800d5c4a - (short)((DAT_800cfc38 + 28000) / 0x1c2);
    puVar18[0xc] = ((byte)(DAT_800cfc6e + 0x100 >> 9) & 7) * '\f';
    puVar18[0xd] = 0;
    cVar14 = DAT_800d5c18 + '(';
    puVar18[4] = cVar14;
    puVar18[5] = cVar14;
    puVar18[6] = cVar14;
    *(short *)(puVar18 + 8) = DAT_800d5c44 + *(short *)(puVar6 + uVar5 * 0x10 + 8);
    *(short *)(puVar18 + 10) = DAT_800d5c46 + *(short *)(puVar6 + uVar5 * 0x10 + 10);
    AddPrim(ot,puVar18);
  }
  p = puVar18 + 0x28;
  if (DAT_800d5c0a < 2) {
    p = puVar18 + 0x78;
  }
  else {
    iVar15 = 2;
    puVar18 = puVar18 + 0x2e;
    sVar11 = 0x198;
    do {
      sVar11 = sVar11 + -0xc2;
      *(undefined2 *)(puVar18 + 2) = 0x98;
      *(short *)(puVar18 + 4) = sVar11;
      iVar15 = iVar15 + -1;
      if (DAT_800d5c19 == '\0') {
        puVar18[-2] = 0x80;
        puVar18[-1] = 0x80;
        *puVar18 = 0x80;
      }
      else {
                    /* Possible PsyQ macro: setLineF2() */
        puVar18[-2] = 0x40;
        puVar18[-1] = 0x40;
        *puVar18 = 0x40;
      }
      if (iVar15 == 0) {
        bVar1 = (&DAT_800a9aec)[DAT_800d5c0a];
      }
      else {
        bVar1 = (&DAT_800a9b04)[DAT_800d5c0a];
      }
      if ((1 < bVar1) && (iVar7 = FUN_80077360(&DAT_800d4908), iVar7 != 0)) {
        AddPrim(ot,p);
      }
      puVar18 = puVar18 + 0x28;
      p = p + 0x28;
    } while (iVar15 != 0);
  }
  iVar15 = 2;
  psVar9 = (short *)(p + 10);
  sVar17 = 0x1a;
  sVar11 = 0x1ea;
  AddPrim(ot,&UNK_800d6c20 + (uint)DAT_800ce5e0 * 0xc);
  do {
    puVar18 = p;
    sVar17 = sVar17 + -5;
    sVar11 = sVar11 + -0xf0;
    iVar15 = iVar15 + -1;
    psVar9[-1] = sVar11;
    *psVar9 = sVar17;
    AddPrim(ot,puVar18);
    psVar9 = psVar9 + 0x14;
    p = puVar18 + 0x28;
  } while (iVar15 != 0);
  puVar10 = (u_short *)(puVar18 + 0x36);
  uVar16 = (uint)(byte)(&DAT_800aaa3c)[(uint)DAT_800d5c0a * 8];
  puVar13 = puVar6 + uVar16 * 0x10;
  do {
    puVar12 = puVar13 + -0x10;
    puVar10[-3] = *(short *)(puVar13 + -0xc) + DAT_800d5c48;
    puVar10[-2] = *(short *)(puVar13 + -10) + DAT_800d5c4a;
    uVar16 = uVar16 - 1;
    if ((DAT_800d5c0a == 2) && (uVar16 == 0xe)) {
      if (DAT_800d481e == 0xe) {
        iVar15 = FUN_80077360(&DAT_800d4920,8);
        if (iVar15 != 0) {
          iVar15 = FUN_80077360(&DAT_800d4920,0xc);
          unaff_s5 = 0x1fa;
          if (iVar15 == 0) {
            unaff_s5 = 0x1fe;
            uVar8 = puVar6[0xe0];
            uVar2 = 0x98;
          }
          else {
            uVar8 = puVar6[0xe0];
            uVar2 = 0x50;
          }
          *(undefined1 *)((int)puVar10 + -1) = uVar2;
          *(undefined1 *)(puVar10 + -1) = uVar8;
        }
        iVar15 = FUN_80077360(&DAT_800d4920,9);
        if (iVar15 != 0) {
          iVar15 = FUN_80077360(&DAT_800d4920,0xb);
          unaff_s5 = 0x1fa;
          if (iVar15 == 0) {
            unaff_s5 = 0x1fe;
            uVar8 = *puVar12;
            uVar2 = 0x50;
          }
          else {
            uVar8 = *puVar12;
            uVar2 = 0x98;
          }
          *(undefined1 *)((int)puVar10 + -1) = uVar2;
          *(undefined1 *)(puVar10 + -1) = uVar8;
        }
      }
      else {
        iVar15 = FUN_80077360(&DAT_800d490c,0x2c);
        if ((iVar15 == 0) &&
           (iVar15 = FUN_80077360(&DAT_800d4924,(&DAT_800aaa3d)[(uint)DAT_800d5c0a * 8]),
           iVar15 == 0)) goto LAB_8006e768;
        iVar15 = FUN_80077360(&DAT_800d4920,0xb);
        if (iVar15 == 0) {
          unaff_s5 = 0x1f2;
          uVar8 = puVar6[0xe0];
                    /* Possible PsyQ macro: setLineG2() */
          *(undefined1 *)((int)puVar10 + -1) = 0x50;
          *(undefined1 *)(puVar10 + -1) = uVar8;
          iVar15 = FUN_80077360(&DAT_800d4920,0xc);
          if (iVar15 != 0) {
            unaff_s5 = 0x1fd;
          }
        }
        else {
          unaff_s5 = 0x1fd;
          uVar8 = puVar6[0xe0];
          *(undefined1 *)((int)puVar10 + -1) = 0x98;
          *(undefined1 *)(puVar10 + -1) = uVar8;
          iVar15 = FUN_80077360(&DAT_800d4920,0xc);
          if (iVar15 != 0) {
            unaff_s5 = 0x1f5;
          }
        }
      }
LAB_8006e750:
      uVar3 = GetClut(0x100,unaff_s5);
      *puVar10 = uVar3;
      AddPrim(ot,p);
    }
    else {
      unaff_s5 = 0x1f5;
      iVar15 = FUN_80077360(&DAT_800d4920,puVar13[-3]);
      if (iVar15 != 0) {
        unaff_s5 = 0x1fa;
      }
      if ((DAT_800d5c0a == uVar4) && (uVar16 == uVar5)) {
        unaff_s5 = unaff_s5 + 1;
      }
      iVar15 = FUN_80077360(&DAT_800d4924,(&DAT_800aaa3d)[(uint)DAT_800d5c0a * 8]);
      if (iVar15 != 0) {
        iVar15 = FUN_80077360(&DAT_800d490c,puVar13[-4]);
        if (iVar15 == 0) {
          if ((DAT_800cfc00 & 1) == 0) {
            bVar1 = puVar13[-2];
          }
          else {
            bVar1 = puVar13[-1];
          }
          if ((((bVar1 & 1) != 0) && ((DAT_800cfbd8 & 0x40000000) == 0)) ||
             (((bVar1 & 2) != 0 && ((DAT_800cfbd8 & 0x40000000) != 0)))) goto LAB_8006e768;
          unaff_s5 = 0x1f2;
          iVar15 = FUN_80077360(&DAT_800d4920,puVar13[-3]);
          if (iVar15 != 0) {
            unaff_s5 = 0x1f7;
          }
        }
        goto LAB_8006e750;
      }
      iVar15 = FUN_80077360(&DAT_800d490c,puVar13[-4]);
      if (iVar15 != 0) goto LAB_8006e750;
    }
LAB_8006e768:
    puVar10 = puVar10 + 0x14;
    p = p + 0x28;
    puVar13 = puVar12;
    if (uVar16 == 0) {
      AddPrim(ot,&UNK_800d6c08 + (uint)DAT_800ce5e0 * 0xc);
      if ((DAT_800cfbd8 & 0x2000000) == 0) {
        FUN_8006dcc0();
      }
      return;
    }
  } while( true );
}


