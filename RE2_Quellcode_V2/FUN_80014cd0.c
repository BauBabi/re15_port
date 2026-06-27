/* FUN_80014cd0 @ 0x80014cd0  (Ghidra headless, raw MIPS overlay) */

void FUN_80014cd0(void)

{
  byte bVar1;
  uint uVar2;
  uint *puVar3;
  bool bVar4;
  short sVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint extraout_v1;
  uint uVar9;
  uint uVar10;
  uint *puVar11;
  uint *puVar12;
  uint *puVar13;
  uint uVar14;
  
  bVar4 = false;
  bVar1 = *(byte *)(DAT_800ce550 + 0xc);
LAB_80014d0c:
  do {
    SpuSetTransferMode(0);
    DAT_800cfbd8 = DAT_800cfbd8 | 0x20000;
    iVar6 = (uint)*(byte *)(DAT_800ce550 + 0xc) * 0xc;
    while (DAT_800c3a84 != 0) {
      FUN_80031f94(1);
    }
    uVar10 = (uint)*(ushort *)(&DAT_8009a4b0 + (uint)bVar1 * 2);
    uVar14 = (uint)*(ushort *)(&DAT_8009a520 + iVar6);
    DAT_800d531e = (&DAT_8009a528)[iVar6];
    iVar6 = -uVar14;
    puVar13 = (uint *)((int)&DAT_801a1000 + iVar6);
    DAT_800d5314 = (uint)*(ushort *)(&DAT_800988a8 + uVar10 * 8) +
                   (uint)(byte)(&DAT_800988aa)[uVar10 * 8] * 0x10000;
    puVar11 = puVar13;
    DAT_800d5308 = uVar14;
    FUN_80012fb8(uVar10,puVar13,3,"DOOR SOUND");
    uVar7 = (uint)puVar13 & 3;
    puVar12 = &DAT_801fb700;
    if (uVar7 == 0) {
      do {
        uVar7 = *puVar13;
        uVar9 = puVar13[1];
        uVar10 = puVar13[2];
        uVar8 = puVar13[3];
        *puVar12 = uVar7;
        puVar12[1] = uVar9;
        puVar12[2] = uVar10;
        puVar12[3] = uVar8;
        puVar13 = puVar13 + 4;
        puVar12 = puVar12 + 4;
      } while (puVar13 != (uint *)(&UNK_801a1c30 + iVar6));
    }
    else {
      uVar9 = extraout_v1;
      do {
        uVar8 = (uint)((int)puVar13 + 3) & 3;
        uVar2 = (uint)puVar13 & 3;
        uVar7 = (*(int *)((undefined *)((int)puVar13 + 3) + -uVar8) << (3 - uVar8) * 8 |
                uVar7 & 0xffffffffU >> (uVar8 + 1) * 8) & -1 << (4 - uVar2) * 8 |
                *(uint *)((int)puVar13 - uVar2) >> uVar2 * 8;
        uVar8 = (uint)((int)puVar13 + 7) & 3;
        uVar2 = (uint)(puVar13 + 1) & 3;
        uVar9 = (*(int *)((undefined *)((int)puVar13 + 7) + -uVar8) << (3 - uVar8) * 8 |
                uVar9 & 0xffffffffU >> (uVar8 + 1) * 8) & -1 << (4 - uVar2) * 8 |
                *(uint *)((int)(puVar13 + 1) - uVar2) >> uVar2 * 8;
        uVar8 = (uint)((int)puVar13 + 0xb) & 3;
        uVar2 = (uint)(puVar13 + 2) & 3;
        uVar10 = (*(int *)((undefined *)((int)puVar13 + 0xb) + -uVar8) << (3 - uVar8) * 8 |
                 uVar10 & 0xffffffffU >> (uVar8 + 1) * 8) & -1 << (4 - uVar2) * 8 |
                 *(uint *)((int)(puVar13 + 2) - uVar2) >> uVar2 * 8;
        uVar8 = (uint)((int)puVar13 + 0xf) & 3;
        uVar2 = (uint)(puVar13 + 3) & 3;
        puVar11 = (uint *)((*(int *)((undefined *)((int)puVar13 + 0xf) + -uVar8) << (3 - uVar8) * 8
                           | (uint)puVar11 & 0xffffffffU >> (uVar8 + 1) * 8) & -1 << (4 - uVar2) * 8
                          | *(uint *)((int)(puVar13 + 3) - uVar2) >> uVar2 * 8);
        uVar8 = (int)puVar12 + 3U & 3;
        puVar3 = (uint *)(((int)puVar12 + 3U) - uVar8);
        *puVar3 = *puVar3 & -1 << (uVar8 + 1) * 8 | uVar7 >> (3 - uVar8) * 8;
        *puVar12 = uVar7;
        uVar8 = (int)puVar12 + 7U & 3;
        puVar3 = (uint *)(((int)puVar12 + 7U) - uVar8);
        *puVar3 = *puVar3 & -1 << (uVar8 + 1) * 8 | uVar9 >> (3 - uVar8) * 8;
        puVar12[1] = uVar9;
        uVar8 = (int)puVar12 + 0xbU & 3;
        puVar3 = (uint *)(((int)puVar12 + 0xbU) - uVar8);
        *puVar3 = *puVar3 & -1 << (uVar8 + 1) * 8 | uVar10 >> (3 - uVar8) * 8;
        puVar12[2] = uVar10;
        uVar8 = (int)puVar12 + 0xfU & 3;
        puVar3 = (uint *)(((int)puVar12 + 0xfU) - uVar8);
        *puVar3 = *puVar3 & -1 << (uVar8 + 1) * 8 | (uint)puVar11 >> (3 - uVar8) * 8;
        puVar12[3] = (uint)puVar11;
        puVar13 = puVar13 + 4;
        puVar12 = puVar12 + 4;
      } while (puVar13 != (uint *)(&UNK_801a1c30 + iVar6));
    }
    uVar10 = (uint)((int)puVar13 + 3) & 3;
    uVar8 = (uint)puVar13 & 3;
    uVar8 = (*(int *)((undefined *)((int)puVar13 + 3) + -uVar10) << (3 - uVar10) * 8 |
            uVar7 & 0xffffffffU >> (uVar10 + 1) * 8) & -1 << (4 - uVar8) * 8 |
            *(uint *)((int)puVar13 - uVar8) >> uVar8 * 8;
    uVar7 = (uint)((int)puVar13 + 7) & 3;
    uVar10 = (uint)(puVar13 + 1) & 3;
    uVar10 = (*(int *)((undefined *)((int)puVar13 + 7) + -uVar7) << (3 - uVar7) * 8 |
             uVar9 & 0xffffffffU >> (uVar7 + 1) * 8) & -1 << (4 - uVar10) * 8 |
             *(uint *)((int)(puVar13 + 1) - uVar10) >> uVar10 * 8;
    uVar7 = (int)puVar12 + 3U & 3;
    puVar11 = (uint *)(((int)puVar12 + 3U) - uVar7);
    *puVar11 = *puVar11 & -1 << (uVar7 + 1) * 8 | uVar8 >> (3 - uVar7) * 8;
    *puVar12 = uVar8;
    uVar7 = (int)puVar12 + 7U & 3;
    puVar11 = (uint *)(((int)puVar12 + 7U) - uVar7);
    *puVar11 = *puVar11 & -1 << (uVar7 + 1) * 8 | uVar10 >> (3 - uVar7) * 8;
    puVar12[1] = uVar10;
    DAT_800dbb78 = &DAT_801fb700;
    DAT_800d75a0 = (uchar *)((int)&DAT_801fb700 + DAT_801fc330);
    if (-1 < DAT_800d4c48) {
      SsVabClose((short)DAT_800d4c48);
      DAT_800d4c48 = -1;
    }
    uVar7 = 0;
    while (sVar5 = SsVabTransCompleted(0), sVar5 == 0) {
      FUN_80031f94(1);
    }
    do {
      sVar5 = SsVabOpenHeadSticky(DAT_800d75a0,0,0x3dc50);
      DAT_800d4c48 = (char)sVar5;
      if (((DAT_800cfb74 & 0x10000) == 0) && (!bVar4)) {
        FUN_8002bda8(2,0);
        bVar4 = true;
      }
      FUN_80031f94(1);
      uVar7 = uVar7 + 1;
      if (0xb4 < uVar7) goto LAB_80014d0c;
    } while (DAT_800d4c48 == -1);
    uVar7 = 0;
    while( true ) {
      FUN_80031f94(1);
      uVar7 = uVar7 + 1;
      if (0xb4 < uVar7) break;
      sVar5 = SsVabTransBody(&UNK_801a1c38 + -uVar14,(short)DAT_800d4c48);
      if (sVar5 != -1) {
        DAT_800a7fac = 0;
        do {
          FUN_8005cf70();
          if (((DAT_800cfb74 & 0x10000) == 0) && (!bVar4)) {
            FUN_8002bda8(2,0);
            bVar4 = true;
          }
          FUN_80031f94(1);
          sVar5 = SsVabTransCompleted(0);
        } while (sVar5 == 0);
        DAT_800cfbd8 = DAT_800cfbd8 & 0xfffdffff;
        return;
      }
    }
  } while( true );
}


