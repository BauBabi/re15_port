/* FUN_8006f1c4 @ 0x8006f1c4  (Ghidra headless, raw MIPS overlay) */

void FUN_8006f1c4(void)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  u_short uVar4;
  uint uVar5;
  undefined1 uVar6;
  short *psVar7;
  short sVar8;
  u_short *puVar9;
  int unaff_s2;
  short sVar10;
  undefined1 *puVar11;
  int iVar13;
  uint uVar14;
  undefined *puVar15;
  undefined *p;
  undefined *puVar16;
  undefined *ot;
  undefined1 *puVar12;
  
  ot = &UNK_800cc1fc + (uint)DAT_800ce5e0 * 0x20;
  puVar15 = &UNK_8019d000;
  if (DAT_800d5c19 == '\0') {
    cVar2 = DAT_800d5c18 + 2;
    if (0x50 < DAT_800d5c18) {
      DAT_800d5c19 = '\x01';
      cVar2 = DAT_800d5c18 + 2;
    }
  }
  else {
    if (DAT_800d5c18 < 10) {
      FUN_8005ba28(0x22b0000,0);
      DAT_800d5c19 = '\0';
    }
    cVar2 = DAT_800d5c18 - 2;
  }
  if (DAT_800ce5e0 != 0) {
    puVar15 = &DAT_8019d014;
  }
  iVar13 = 2;
  psVar7 = (short *)(puVar15 + 0x82);
  sVar10 = 0x1a;
  DAT_800d5c18 = cVar2;
  uVar5 = FUN_8006eae8(DAT_800d5c0a,(&DAT_800a9ba0)[DAT_800d69f2]);
  sVar8 = 0x1ea;
  puVar16 = (&PTR_DAT_800aaa38)[(uint)DAT_800d5c0a * 2];
  puVar15 = puVar15 + 0x78;
  do {
    p = puVar15;
    sVar10 = sVar10 + -5;
    sVar8 = sVar8 + -0xf0;
    iVar13 = iVar13 + -1;
    psVar7[-1] = sVar8;
    *psVar7 = sVar10;
    AddPrim(ot,p);
    psVar7 = psVar7 + 0x14;
    puVar15 = p + 0x28;
  } while (iVar13 != 0);
  uVar14 = (uint)(byte)(&DAT_800aaa3c)[(uint)DAT_800d5c0a * 8];
  puVar9 = (u_short *)(p + 0x36);
  puVar12 = puVar16 + uVar14 * 0x10;
  do {
    puVar11 = puVar12 + -0x10;
    puVar9[-3] = *(short *)(puVar12 + -0xc) + DAT_800d5c48;
    puVar9[-2] = *(short *)(puVar12 + -10) + DAT_800d5c4a;
    uVar14 = uVar14 - 1;
    if ((DAT_800d5c0a == 2) && (uVar14 == 0xe)) {
      if (DAT_800d481e == 0xe) {
        iVar13 = FUN_80077360(&DAT_800d4920,8);
        if (iVar13 != 0) {
          iVar13 = FUN_80077360(&DAT_800d4920,0xc);
          unaff_s2 = 0x1fa;
          if (iVar13 == 0) {
            unaff_s2 = 0x1fe;
            uVar6 = puVar16[0xe0];
            uVar3 = 0x98;
          }
          else {
            uVar6 = puVar16[0xe0];
            uVar3 = 0x50;
          }
          *(undefined1 *)((int)puVar9 + -1) = uVar3;
          *(undefined1 *)(puVar9 + -1) = uVar6;
        }
        iVar13 = FUN_80077360(&DAT_800d4920,9);
        if (iVar13 != 0) {
          iVar13 = FUN_80077360(&DAT_800d4920,0xb);
          unaff_s2 = 0x1fa;
          if (iVar13 == 0) {
            unaff_s2 = 0x1fe;
            uVar6 = *puVar11;
            uVar3 = 0x50;
          }
          else {
            uVar6 = *puVar11;
            uVar3 = 0x98;
          }
          *(undefined1 *)((int)puVar9 + -1) = uVar3;
          *(undefined1 *)(puVar9 + -1) = uVar6;
        }
      }
      else {
        iVar13 = FUN_80077360(&DAT_800d490c,0x2c);
        if (iVar13 == 0) goto LAB_8006f620;
        iVar13 = FUN_80077360(&DAT_800d4920,0xb);
        if (iVar13 == 0) {
          unaff_s2 = 0x1f2;
          uVar6 = puVar16[0xe0];
                    /* Possible PsyQ macro: setLineG2() */
          *(undefined1 *)((int)puVar9 + -1) = 0x50;
          *(undefined1 *)(puVar9 + -1) = uVar6;
          iVar13 = FUN_80077360(&DAT_800d4920,0xc);
          if (iVar13 != 0) {
            unaff_s2 = 0x1fd;
          }
        }
        else {
          unaff_s2 = 0x1fd;
          uVar6 = puVar16[0xe0];
          *(undefined1 *)((int)puVar9 + -1) = 0x98;
          *(undefined1 *)(puVar9 + -1) = uVar6;
          iVar13 = FUN_80077360(&DAT_800d4920,0xc);
          if (iVar13 != 0) {
            unaff_s2 = 0x1f5;
          }
        }
      }
LAB_8006f608:
      uVar4 = GetClut(0x100,unaff_s2);
      *puVar9 = uVar4;
      AddPrim(ot,puVar15);
    }
    else {
      unaff_s2 = 0x1f5;
      iVar13 = FUN_80077360(&DAT_800d4920,puVar12[-3]);
      if (iVar13 != 0) {
        unaff_s2 = 0x1fa;
      }
      if (uVar14 == uVar5) {
        if (DAT_800d5c19 == '\0') {
          unaff_s2 = unaff_s2 + 1;
        }
        else {
          uVar6 = puVar12[-3];
LAB_8006f5d8:
          unaff_s2 = 0x1f2;
          iVar13 = FUN_80077360(&DAT_800d4920,uVar6);
          if (iVar13 != 0) {
            unaff_s2 = 0x1f7;
          }
        }
        goto LAB_8006f608;
      }
      iVar13 = FUN_80077360(&DAT_800d4924,(&DAT_800aaa3d)[(uint)DAT_800d5c0a * 8]);
      if (iVar13 == 0) {
        iVar13 = FUN_80077360(&DAT_800d490c,puVar12[-4]);
        if (iVar13 != 0) goto LAB_8006f608;
      }
      else {
        iVar13 = FUN_80077360(&DAT_800d490c,puVar12[-4]);
        if (iVar13 != 0) goto LAB_8006f608;
        if ((DAT_800cfc00 & 1) == 0) {
          bVar1 = puVar12[-2];
        }
        else {
          bVar1 = puVar12[-1];
        }
        if ((((bVar1 & 1) == 0) || ((DAT_800cfbd8 & 0x40000000) != 0)) &&
           (((bVar1 & 2) == 0 || ((DAT_800cfbd8 & 0x40000000) == 0)))) {
          uVar6 = puVar12[-3];
          goto LAB_8006f5d8;
        }
      }
    }
LAB_8006f620:
    puVar9 = puVar9 + 0x14;
    puVar15 = puVar15 + 0x28;
    puVar12 = puVar11;
    if (uVar14 == 0) {
      AddPrim(ot,&UNK_800d6c08 + (uint)DAT_800ce5e0 * 0xc);
      if ((DAT_800cfbd8 & 0x2000000) == 0) {
        FUN_8006dcc0();
      }
      return;
    }
  } while( true );
}


