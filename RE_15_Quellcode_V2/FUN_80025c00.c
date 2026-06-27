undefined4 FUN_80025c00(int param_1,int param_2,undefined4 param_3,undefined1 param_4)

{
  undefined1 *puVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  undefined1 *puVar5;
  int iVar6;
  uint unaff_s1;
  byte bVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint unaff_s5;
  uint uVar11;
  uint uVar12;
  uint unaff_s7;
  undefined1 auStack_230 [4];
  undefined1 auStack_22c [4];
  undefined1 auStack_228 [4];
  char acStack_224 [318];
  char local_e6;
  int local_e4;
  undefined1 auStack_a0 [72];
  int local_58;
  int local_50;
  undefined1 local_48;
  char local_40;
  byte local_38;
  int local_30;
  
  iVar6 = 0;
  uVar12 = 0;
  local_58 = param_1;
  local_50 = param_2;
  local_48 = param_4;
  FUN_800264e8(param_4);
  uVar8 = 0;
  DAT_801ff598 = local_50;
  do {
    cVar3 = '\x13';
    if (((DAT_800ac762 & 0x5100) == 0) && (cVar3 = '\x05', local_40 != '\0')) {
      local_40 = local_40 + -1;
      uVar10 = uVar8;
    }
    else {
      local_40 = cVar3;
      if ((iVar6 == 6) || ((iVar6 == 2 || ((DAT_800ac75c & 0x100) != 0)))) {
        if ((DAT_800ac760 & 0x1000) != 0) {
          uVar12 = 0;
          uVar8 = uVar8 - 1;
        }
        iVar4 = uVar8 << 0x18;
        if ((DAT_800ac760 & 0x4100) != 0) {
          uVar12 = 0;
          uVar8 = uVar8 + 1;
          iVar4 = uVar8 * 0x1000000;
        }
        if (iVar4 < 0) {
          uVar8 = unaff_s5;
        }
        if ((unaff_s5 & 0xff) < (uVar8 & 0xff)) {
          uVar8 = 0;
        }
      }
      else {
        if (((DAT_800ac760 & 0x1000) != 0) && ((uVar8 & 0xff) != 0)) {
          uVar12 = 0;
          uVar8 = uVar8 - 1;
        }
        if (((DAT_800ac760 & 0x4100) != 0) && ((uVar8 & 0xff) < (unaff_s5 & 0xff))) {
          uVar12 = 0;
          uVar8 = uVar8 + 1;
        }
      }
      uVar10 = uVar8;
      if (((unaff_s5 & 0xff) != 0) && ((unaff_s5 & 0xff) < (uVar8 & 0xff))) {
        uVar10 = unaff_s5;
      }
    }
    uVar12 = uVar12 + 1;
    if ((uVar10 & 0xff) != (uint)local_38) {
      uVar12 = 1;
    }
    uVar12 = uVar12 & 0x1f;
    DAT_801ff405 = (undefined1)(uVar12 >> 4);
    bVar7 = (byte)uVar10;
    local_38 = bVar7;
    uVar8 = uVar10;
    bVar2 = DAT_801ff404;
    switch(iVar6) {
    case 0:
      iVar4 = FUN_80026dd4();
      if (iVar4 - 1U < 2) {
        iVar6 = 4;
        bVar2 = (byte)iVar4 & 1;
      }
      else {
        iVar6 = 2;
        bVar2 = DAT_801ff404;
        if (iVar4 == 3) {
          DAT_801ff404 = 0;
          goto LAB_800260ac;
        }
      }
      break;
    case 1:
      unaff_s5 = 1;
      if (((DAT_800ac76c & 0x4000) == 0) && ((DAT_800ac75c & 0x800) == 0)) {
LAB_80025ebc:
        if ((DAT_800ac76c & 0x8000) != 0) goto LAB_80026468;
      }
      else {
        if ((uVar10 & 0xff) == 1) goto LAB_80026468;
LAB_800262f8:
        unaff_s5 = 1;
        iVar6 = 0;
        uVar8 = 0;
      }
      break;
    case 2:
      unaff_s5 = 2;
      if (((DAT_800ac76c & 0x4000) == 0) && ((DAT_800ac75c & 0x800) == 0)) goto LAB_80025ebc;
      iVar6 = 4;
      bVar2 = bVar7;
      if ((uVar10 & 0xff) == 2) goto LAB_80026468;
      break;
    case 3:
      unaff_s5 = 0;
      if ((DAT_800ac76c & 0xc000) == 0) {
        uVar8 = 0;
        if ((DAT_800ac75c & 0x800) != 0) {
          iVar6 = 1;
        }
      }
      else {
LAB_800262e4:
        iVar6 = 1;
        uVar8 = 0;
      }
      break;
    case 4:
      iVar6 = 5;
      FUN_800c0158(0x140,0xc0,7,0);
      FUN_80029ac8(1);
      bVar2 = DAT_801ff404;
      break;
    case 5:
      unaff_s5 = 5;
      FUN_800275f0(DAT_801ff404);
      unaff_s1 = FUN_800276b0(DAT_801ff404);
      if ((unaff_s1 & 0xff) == 0) {
        unaff_s1 = FUN_80026ca8(&DAT_801ff407,local_50);
        FUN_80027488(DAT_801ff404,auStack_230,&DAT_80010710);
        local_30 = local_e4;
      }
      uVar10 = unaff_s1 & 0xff;
      if (uVar10 == 1) {
LAB_800260ac:
        iVar6 = 3;
        unaff_s1 = 4;
        bVar2 = DAT_801ff404;
      }
      else {
        bVar2 = DAT_801ff404;
        if (uVar10 < 2) {
          if (uVar10 == 0) {
            if ((local_58 != 0) || (iVar6 = 3, DAT_801ff556 != '\0')) {
              uVar9 = 0;
              uVar11 = unaff_s5;
              goto LAB_80026440;
            }
            unaff_s1 = 3;
          }
        }
        else if (uVar10 == 2) {
          iVar6 = 10;
          uVar8 = 0;
        }
        else if (uVar10 == 3) {
          iVar6 = 3;
          unaff_s1 = 1;
        }
      }
      break;
    case 6:
      unaff_s5 = 5;
      if (((DAT_800ac76c & 0x4000) == 0) && ((DAT_800ac75c & 0x800) == 0)) {
        if ((DAT_800ac76c & 0x8000) != 0) {
          iVar6 = 0;
          uVar8 = 0;
        }
      }
      else {
        if ((uVar10 & 0xff) == 5) {
LAB_80026468:
          FUN_80026594(local_58,local_48);
                    /* WARNING: Read-only address (ram,0x801ff556) is written */
          return 1;
        }
        if ((local_58 == 0) &&
           (FUN_80027488(DAT_801ff404,auStack_230,"BISLPS-00222*"), local_e6 == '\0')) {
          FUN_80061fc0(0);
          unaff_s1 = FUN_800275f0(DAT_801ff404);
          iVar6 = 7;
          bVar2 = DAT_801ff404;
          if ((unaff_s1 & 0xff) != 0) goto LAB_800260ac;
        }
        else {
          iVar6 = 7;
          bVar2 = DAT_801ff404;
        }
      }
      break;
    case 7:
      if (local_58 == 0) {
        iVar6 = (uVar10 & 0xff) * 0x16;
        puVar5 = auStack_230 + iVar6;
        puVar1 = auStack_230 + iVar6 + 3;
        uVar9 = (uint)puVar1 & 3;
        *(uint *)(puVar1 + -uVar9) =
             *(uint *)(puVar1 + -uVar9) & -1 << (uVar9 + 1) * 8 | 0x4c534942U >> (3 - uVar9) * 8;
        uVar9 = (uint)puVar5 & 3;
        *(uint *)(puVar5 + -uVar9) =
             *(uint *)(puVar5 + -uVar9) & 0xffffffffU >> (4 - uVar9) * 8 | 0x4c534942 << uVar9 * 8;
        puVar1 = auStack_230 + iVar6 + 7;
        uVar9 = (uint)puVar1 & 3;
        *(uint *)(puVar1 + -uVar9) =
             *(uint *)(puVar1 + -uVar9) & -1 << (uVar9 + 1) * 8 | 0x302d5350U >> (3 - uVar9) * 8;
        puVar1 = auStack_230 + iVar6 + 4;
        uVar9 = (uint)puVar1 & 3;
        *(uint *)(puVar1 + -uVar9) =
             *(uint *)(puVar1 + -uVar9) & 0xffffffffU >> (4 - uVar9) * 8 | 0x302d5350 << uVar9 * 8;
        puVar1 = auStack_230 + iVar6 + 0xb;
        uVar9 = (uint)puVar1 & 3;
        *(uint *)(puVar1 + -uVar9) =
             *(uint *)(puVar1 + -uVar9) & -1 << (uVar9 + 1) * 8 | 0x32323230U >> (3 - uVar9) * 8;
        puVar1 = auStack_230 + iVar6 + 8;
        uVar9 = (uint)puVar1 & 3;
        *(uint *)(puVar1 + -uVar9) =
             *(uint *)(puVar1 + -uVar9) & 0xffffffffU >> (4 - uVar9) * 8 | 0x32323230 << uVar9 * 8;
        auStack_230[iVar6 + 0xc] = 0x30;
        auStack_230[iVar6 + 0xd] = 0;
        auStack_230[iVar6 + 0xc] = bVar7 + auStack_230[iVar6 + 0xc];
        iVar6 = 0xd;
        if ((byte)(&DAT_801ff407)[uVar10 & 0xff] < 5) {
          uVar8 = 0;
          unaff_s7 = uVar10;
        }
        else {
          iVar6 = 8;
          if (local_30 == 0x1e000) {
            unaff_s1 = 2;
            goto LAB_80026284;
          }
        }
      }
      else {
        iVar6 = 9;
        if ((&DAT_801ff407)[uVar10 & 0xff] == '\x05') {
          unaff_s1 = 3;
          goto LAB_80026284;
        }
      }
      break;
    case 8:
      uVar10 = uVar10 & 0xff;
      FUN_80026f48();
      FUN_8004ee38(local_50 + uVar10 * 0x80 + 0x1430,&DAT_800b0fbc,0x1230);
      FUN_80026e54(auStack_a0,&DAT_800b0dbc);
      cVar3 = FUN_800271a8(&DAT_800b0dbc,DAT_801ff404,auStack_230 + uVar10 * 0x16,auStack_a0,0x800);
      iVar6 = 3;
      if (cVar3 == '\0') {
        (&DAT_801ff407)[uVar10] = 0;
        DAT_800b0fbd = DAT_800b0fbd + '\x01';
LAB_800264a0:
        FUN_80026594(local_58,local_48);
        return 0;
      }
      unaff_s1 = 5;
      bVar2 = DAT_801ff404;
      break;
    case 9:
      cVar3 = FUN_80027368(local_50,&DAT_801ff40c +
                                    (uint)(byte)(&DAT_801ff407)[uVar10 & 0xff] * 0x16,0x800,0);
      unaff_s1 = 6;
      if (cVar3 == '\0') {
        FUN_8004ee38(&DAT_800b0dbc,local_50,0x1430);
        goto LAB_800264a0;
      }
LAB_80026284:
      iVar6 = 0xc;
      unaff_s7 = uVar10;
      bVar2 = DAT_801ff404;
      break;
    case 10:
      unaff_s5 = 1;
      if (((DAT_800ac76c & 0x4000) == 0) && ((DAT_800ac75c & 0x800) == 0)) {
        if ((DAT_800ac76c & 0x8000) != 0) goto LAB_800262f8;
      }
      else {
        iVar6 = 0xb;
        if ((uVar10 & 0xff) != 0) goto LAB_800262e4;
      }
      break;
    case 0xb:
      cVar3 = FUN_800275f0(DAT_801ff404);
      if (cVar3 == '\0') {
        unaff_s1 = FUN_800276b0(DAT_801ff404);
        if ((unaff_s1 & 0xff) == 2) {
          iVar4 = FUN_80027724(DAT_801ff404);
          iVar6 = 4;
          bVar2 = DAT_801ff404;
          if (iVar4 == 0) break;
        }
        unaff_s1 = 0;
        iVar6 = 3;
        bVar2 = DAT_801ff404;
      }
      else {
        unaff_s1 = (uint)(cVar3 == '\x01') << 2;
        iVar6 = 3;
        uVar8 = 0;
        bVar2 = DAT_801ff404;
      }
      break;
    case 0xc:
      if (((DAT_800ac76c & 0xc000) != 0) || (uVar8 = 0, (DAT_800ac75c & 0x800) != 0)) {
        iVar6 = 6;
        uVar8 = unaff_s7;
      }
      FUN_800c0158(0x140,0xc4,unaff_s1 + 0x11,0x10);
      unaff_s5 = uVar8;
      bVar2 = DAT_801ff404;
      break;
    case 0xd:
      uVar9 = unaff_s7;
      uVar11 = unaff_s7;
      if (((DAT_800ac76c & 0x4000) == 0) && ((DAT_800ac75c & 0x800) == 0)) {
        unaff_s5 = 1;
        if ((DAT_800ac76c & 0x8000) != 0) {
LAB_80026440:
          iVar6 = 6;
          uVar8 = uVar9;
          unaff_s5 = uVar11;
          bVar2 = DAT_801ff404;
        }
      }
      else {
        if ((uVar10 & 0xff) == 1) goto LAB_80026440;
        iVar6 = 8;
        uVar8 = unaff_s7;
        unaff_s5 = unaff_s7;
      }
    }
    DAT_801ff404 = bVar2;
    FUN_80026658(iVar6,local_58,uVar8 & 0xff,unaff_s1 & 0xff);
    FUN_80029ac8(1);
  } while( true );
}
