void FUN_800392d4(void)

{
  undefined1 uVar1;
  uint uVar2;
  uint *puVar3;
  char cVar4;
  byte bVar5;
  short sVar6;
  ushort *puVar7;
  ushort uVar8;
  int iVar9;
  uint *puVar10;
  uint *puVar11;
  int iVar12;
  short *psVar13;
  undefined2 *puVar14;
  undefined2 *puVar15;
  ushort *puVar16;
  ushort uVar17;
  ushort uVar18;
  byte *pbVar19;
  short sVar20;
  uint uVar21;
  
  puVar10 = *(uint **)(DAT_800b0fe4 * 0x20 + *(int *)(DAT_800ac778 + 0x24) + 0x1c);
  uVar2 = *puVar10;
  if (uVar2 == 0xffffffff) {
    *DAT_800ac778 = 0;
  }
  else {
    uVar21 = uVar2 & 0xffff;
    puVar11 = puVar10 + 1;
    puVar3 = puVar11 + uVar21 * 2;
    *DAT_800ac778 = (char)(uVar2 >> 0x10);
    cVar4 = DAT_800ac778[7];
    pbVar19 = DAT_800b2584;
    do {
      cVar4 = cVar4 + -1;
      *pbVar19 = 0;
      pbVar19 = pbVar19 + 4;
    } while (cVar4 != '\0');
    bVar5 = 0;
    if (uVar21 != 0) {
      psVar13 = (short *)((int)puVar10 + 10);
      iVar9 = DAT_800bb4d8;
      iVar12 = DAT_800bb4d4;
      pbVar19 = DAT_800b2584;
      do {
        uVar8 = 0;
        if ((ushort)*puVar11 != 0) {
          puVar15 = (undefined2 *)(iVar9 + 0x1a);
          puVar14 = (undefined2 *)(iVar12 + 0x1a);
          puVar7 = (ushort *)(pbVar19 + 2);
          puVar16 = (ushort *)((int)puVar3 + 10);
          do {
            *pbVar19 = *pbVar19 | 1;
            *(byte *)((int)puVar7 + -1) = bVar5 + 1;
            *puVar7 = puVar16[-3];
            uVar2 = *puVar3;
            sVar6 = psVar13[-1] + (ushort)(byte)puVar16[-4];
            uVar1 = *(undefined1 *)((int)puVar16 + -9);
            sVar20 = *psVar13 + (ushort)*(byte *)((int)puVar16 + -7);
            if ((puVar16[-2] & 0xf000) == 0) {
              uVar17 = puVar16[-1];
              uVar18 = *puVar16;
              puVar16 = puVar16 + 6;
              puVar3 = puVar3 + 3;
            }
            else {
              uVar17 = (puVar16[-2] >> 0xc) << 3;
              puVar16 = puVar16 + 4;
              puVar3 = puVar3 + 2;
              uVar18 = uVar17;
            }
            SetShadeTex((void *)(iVar12 + 0xc),1);
            SetShadeTex((void *)(iVar9 + 0xc),1);
            pbVar19 = pbVar19 + 4;
            uVar8 = uVar8 + 1;
            *(undefined1 *)(puVar14 + -5) = 0x80;
            *(undefined1 *)((int)puVar14 + -9) = 0x80;
            *(undefined1 *)(puVar14 + -4) = 0x80;
            *(undefined1 *)(puVar15 + -5) = 0x80;
            *(undefined1 *)((int)puVar15 + -9) = 0x80;
            *(undefined1 *)(puVar15 + -4) = 0x80;
            puVar7 = puVar7 + 2;
            puVar14[-2] = sVar20;
            puVar14[-3] = sVar6;
            puVar15[-3] = sVar6;
            puVar15[-2] = sVar20;
            *(char *)(puVar14 + -1) = (char)uVar2;
            *(undefined1 *)((int)puVar14 + -1) = uVar1;
            *(char *)(puVar15 + -1) = (char)uVar2;
            *(undefined1 *)((int)puVar15 + -1) = uVar1;
            puVar14[1] = uVar17;
            puVar14[2] = uVar18;
            puVar15[1] = uVar17;
            puVar15[2] = uVar18;
            *puVar15 = 0x7800;
            *puVar14 = 0x7800;
            puVar14 = puVar14 + 0x10;
            puVar15 = puVar15 + 0x10;
            iVar12 = iVar12 + 0x20;
            iVar9 = iVar9 + 0x20;
          } while ((uVar8 & 0xff) < (ushort)*puVar11);
        }
        psVar13 = psVar13 + 4;
        bVar5 = bVar5 + 1;
        puVar11 = puVar11 + 2;
      } while (bVar5 < uVar21);
    }
  }
  return;
}
