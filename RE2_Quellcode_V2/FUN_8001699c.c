/* FUN_8001699c @ 0x8001699c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001699c(void)

{
  ushort uVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint *puVar5;
  ushort *puVar6;
  undefined *puVar7;
  uint uVar8;
  MATRIX *m;
  int iVar9;
  undefined4 uVar10;
  undefined *puVar11;
  undefined1 *puVar12;
  undefined4 uVar13;
  undefined1 auStack_b0 [48];
  MATRIX local_80 [2];
  
  puVar12 = auStack_b0;
  iVar9 = 0x32;
  puVar7 = &UNK_800ce698;
  uVar10 = 0x1000;
  m = local_80;
  uVar8 = 0xffffff;
  puVar6 = &DAT_800ce69e;
  puVar11 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  uVar13 = DAT_800ce338;
  do {
    if (((puVar6[4] & 4) != 0) && ((puVar6[4] & 8) == 0)) {
      uVar1 = puVar6[5];
      *(undefined4 *)(puVar12 + 0x10) = uVar10;
      *(undefined4 *)(puVar12 + 0x14) = 0;
      *(undefined4 *)(puVar12 + 0x18) = uVar10;
      *(undefined4 *)(puVar12 + 0x1c) = 0;
      *(undefined4 *)(puVar12 + 0x20) = uVar10;
      RotMatrixY((int)(short)uVar1,(MATRIX *)(puVar12 + 0x10));
      *(int *)(puVar12 + 0x24) = (int)(short)puVar6[1];
      *(int *)(puVar12 + 0x28) = (int)(short)puVar6[2];
      *(int *)(puVar12 + 0x2c) = (int)(short)puVar6[3];
      FUN_8002ce94(&DAT_800dcba8,puVar12 + 0x10,m);
      SetRotMatrix(m);
      SetTransMatrix(m);
      uVar4 = (uint)(short)puVar6[-1];
      *(ushort *)(puVar12 + 0x54) = *puVar6;
      *(ushort *)(puVar12 + 0x5c) = *puVar6;
      uVar1 = *puVar6;
      *(uint *)(puVar12 + 0x50) = ~uVar4 - 1;
      *(uint *)(puVar12 + 0x58) = uVar4;
      *(uint *)(puVar12 + 0x60) = ~uVar4 - 1;
      *(uint *)(puVar12 + 0x68) = uVar4;
      *(ushort *)(puVar12 + 100) = ~uVar1 - 1;
      *(ushort *)(puVar12 + 0x6c) = ~uVar1 - 1;
      *(undefined4 *)(puVar12 + 0x70) = *(undefined4 *)(puVar12 + 0x24);
      *(undefined4 *)(puVar12 + 0x74) = *(undefined4 *)(puVar12 + 0x28);
      *(undefined4 *)(puVar12 + 0x78) = *(undefined4 *)(puVar12 + 0x2c);
      *(undefined4 *)(puVar12 + 0x7c) = *(undefined4 *)(puVar12 + 0x30);
      puVar5 = (uint *)(puVar7 + (uint)DAT_800ce5e0 * 0x28 + 0x18);
      iVar2 = FUN_8002c820(puVar12 + 0x70,uVar13);
      if (iVar2 != 0) {
        gte_ldv3((SVECTOR *)(puVar12 + 0x50),(SVECTOR *)(puVar12 + 0x58),(SVECTOR *)(puVar12 + 0x60)
                );
        gte_rtpt_b();
        gte_stsxy3((long *)(puVar5 + 2),(long *)(puVar5 + 4),(long *)(puVar5 + 6));
        gte_ldv0((SVECTOR *)(puVar12 + 0x68));
        gte_rtps();
        gte_stsxy((long *)(puVar5 + 8));
        gte_avsz4();
        gte_stotz((long *)(puVar12 + 0x80));
        if (0x1fff < *(uint *)(puVar12 + 0x80)) {
          *(undefined4 *)(puVar12 + 0x80) = 0x1fff;
        }
        puVar3 = (uint *)(puVar11 + (*(uint *)(puVar12 + 0x80) >> 3) * 4);
                    /* Probable PsyQ macro: addPrim(). */
        *puVar5 = *puVar5 & 0xff000000 | *puVar3 & uVar8;
        *puVar3 = *puVar3 & 0xff000000 | (uint)puVar5 & uVar8;
      }
    }
    puVar6 = puVar6 + 0x34;
    iVar9 = iVar9 + -1;
    puVar7 = puVar7 + 0x68;
  } while (iVar9 != 0);
  return;
}


