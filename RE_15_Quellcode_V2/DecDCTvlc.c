int DecDCTvlc(u_long *bs,u_long *buf)

{
  bool bVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  u_long uVar4;
  undefined2 *puVar5;
  undefined *puVar6;
  uint uVar7;
  uint uVar8;
  u_long *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  int iVar17;
  uint uVar18;
  undefined2 *puVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  
  if (bs == (u_long *)0x0) {
    puVar19 = DAT_8006e0a8 + VLC_OBJ_0;
    uVar7 = DAT_8006e0ac;
    uVar8 = DAT_8006e0b0;
    puVar9 = VLC_OBJ_374;
    puVar5 = DAT_8006e0a8;
    iVar17 = DAT_8006e0b4;
    uVar18 = DAT_8006e0b8;
    iVar20 = DAT_8006e0c0;
    iVar21 = DAT_8006e0c4;
    iVar22 = _card_load;
    goto VLC_OBJ_204;
  }
  iVar20 = 0;
  iVar21 = 0;
  iVar22 = 0;
  puVar19 = (undefined2 *)((int)buf + VLC_OBJ_0 * 2);
  uVar2 = *(undefined2 *)((int)bs + 2);
  uVar4 = bs[2];
  uVar3 = *(undefined2 *)((int)bs + 10);
  iVar17 = (uint)(ushort)bs[1] << 10;
  uVar18 = (uint)(-1 < (int)(*(ushort *)((int)bs + 6) - 3));
  puVar9 = bs + 3;
  uVar8 = 0;
  *(short *)buf = (short)*bs;
  *(undefined2 *)((int)buf + 2) = uVar2;
  uVar15 = CONCAT22((short)uVar4,uVar3);
  puVar11 = (undefined2 *)((int)buf + 2);
VLC_OBJ_E8:
  if (uVar18 == 0) {
    if (uVar15 >> 0x16 == 0x1ff) goto VLC_OBJ_30C;
    uVar7 = uVar15 << 10;
    uVar14 = uVar8 + 10;
    uVar8 = uVar14 & 0xf;
    if ((uVar14 & 0x10) != 0) {
      uVar4 = *puVar9;
      puVar9 = (u_long *)((int)puVar9 + 2);
      uVar7 = uVar7 | (uint)(ushort)uVar4 << uVar8;
    }
    puVar11[1] = (ushort)iVar17 | (ushort)(uVar15 >> 0x16);
  }
  else {
    if (uVar15 >> 0x16 == 0x3ff) {
VLC_OBJ_30C:
      iVar17 = 0x40;
      do {
        puVar11 = puVar11 + 1;
        *puVar11 = 0xfe00;
        bVar1 = iVar17 != 0;
        iVar17 = iVar17 + -1;
      } while (bVar1);
      setCopReg(0,Status,Status | 0x20000,0);
      return 0;
    }
    puVar6 = &DAT_8007e9b4;
    if (-1 < (int)(uVar18 - 3)) {
      puVar6 = &DAT_8007e5b4;
    }
    uVar14 = (uint)*(ushort *)(puVar6 + (uVar15 >> 0x18) * 4);
    uVar16 = (uint)*(ushort *)((int)(puVar6 + (uVar15 >> 0x18) * 4) + 2);
    uVar12 = 0;
    uVar7 = uVar15 << (uVar14 & 0x1f);
    if (uVar16 != 0) {
      uVar12 = uVar7 >> (0x20 - uVar16 & 0x1f);
      bVar1 = -1 < (int)uVar7;
      uVar7 = uVar7 << (uVar16 & 0x1f);
      if (bVar1) {
        uVar12 = uVar12 - (0xffffffffU >> (0x20 - uVar16 & 0x1f));
      }
      uVar8 = uVar8 + uVar16;
    }
    uVar14 = uVar8 + uVar14;
    uVar8 = uVar14 & 0xf;
    if ((uVar14 & 0x10) != 0) {
      uVar4 = *puVar9;
      puVar9 = (u_long *)((int)puVar9 + 2);
      uVar7 = uVar7 | (uint)(ushort)uVar4 << uVar8;
    }
    uVar15 = iVar22 + uVar12;
    if ((int)(uVar18 - 2) < 1) {
      uVar15 = iVar21 + uVar12;
      if (uVar18 == 2) {
        iVar21 = iVar21 + uVar12;
      }
      else {
        uVar15 = iVar20 + uVar12;
        iVar20 = iVar20 + uVar12;
      }
    }
    else {
      iVar22 = iVar22 + uVar12;
    }
    bVar1 = uVar18 == 6;
    puVar11[1] = (ushort)iVar17 | (ushort)((uVar15 & 0xff) << 2);
    uVar18 = uVar18 + 1;
    if (bVar1) {
      uVar18 = 1;
    }
  }
  puVar5 = puVar11 + 2;
  if (-1 < (int)puVar11 + (2 - (int)puVar19)) {
    VLC_OBJ_374 = puVar9;
    DAT_8006e0a8 = puVar11 + 2;
    DAT_8006e0ac = uVar7;
    DAT_8006e0b0 = uVar8;
    DAT_8006e0b4 = iVar17;
    DAT_8006e0b8 = uVar18;
    DAT_8006e0c0 = iVar20;
    DAT_8006e0c4 = iVar21;
    _card_load = iVar22;
    return 1;
  }
VLC_OBJ_204:
  puVar10 = puVar5;
  iVar13 = (uVar7 >> 0x13) * 8;
  uVar14 = *(uint *)(&DAT_8007edb4 + iVar13);
  if (uVar14 == 0) {
    uVar7 = uVar7 << 8;
    uVar15 = uVar8 + 8;
    uVar8 = uVar15 & 0xf;
    if ((uVar15 & 0x10) != 0) {
      uVar4 = *puVar9;
      puVar9 = (u_long *)((int)puVar9 + 2);
      uVar7 = uVar7 | (uint)(ushort)uVar4 << uVar8;
    }
    uVar14 = *(uint *)(&DAT_8008edb4 + (uVar7 >> 0x17) * 4);
    uVar12 = 0;
  }
  else {
    uVar12 = *(uint *)(&DAT_8007edb8 + iVar13);
  }
  uVar7 = uVar7 << (uVar14 & 0x1f);
  uVar15 = uVar8 + (uVar14 & 0xff);
  uVar8 = uVar15 & 0xf;
  if ((uVar15 & 0x10) != 0) {
    uVar4 = *puVar9;
    puVar9 = (u_long *)((int)puVar9 + 2);
    uVar7 = uVar7 | (uint)(ushort)uVar4 << uVar8;
  }
  puVar11 = puVar10;
  if (uVar14 >> 0x10 == 0x7c1f) goto VLC_OBJ_2E8;
  *puVar10 = (short)(uVar14 >> 0x10);
  uVar15 = uVar7;
  if (uVar14 >> 0x10 != 0xfe00) goto code_r0x8006dfd0;
  goto VLC_OBJ_E8;
code_r0x8006dfd0:
  puVar11 = puVar10 + 1;
  puVar5 = puVar11;
  if (uVar12 != 0) {
    if ((uVar12 & 0xffff) != 0x7c1f) {
      *puVar11 = (short)uVar12;
      if ((uVar12 & 0xffff) == 0xfe00) goto VLC_OBJ_E8;
      uVar14 = uVar12 >> 0x10;
      puVar11 = puVar10 + 2;
      puVar5 = puVar11;
      if (uVar14 == 0) goto VLC_OBJ_204;
      if (uVar14 != 0x7c1f) {
        *puVar11 = (short)(uVar12 >> 0x10);
        if (uVar14 != 0xfe00) {
          puVar5 = puVar10 + 3;
          goto VLC_OBJ_204;
        }
        goto VLC_OBJ_E8;
      }
    }
VLC_OBJ_2E8:
    *puVar11 = (short)(uVar7 >> 0x10);
    uVar4 = *puVar9;
    puVar9 = (u_long *)((int)puVar9 + 2);
    uVar7 = uVar7 << 0x10 | (uint)(ushort)uVar4 << uVar8;
    puVar5 = puVar11 + 1;
  }
  goto VLC_OBJ_204;
}
