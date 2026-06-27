/* FUN_80077924 @ 0x80077924  (Ghidra headless, raw MIPS overlay) */

void FUN_80077924(void)

{
  short sVar1;
  short sVar2;
  ushort uVar3;
  int iVar4;
  undefined1 *puVar5;
  byte *pbVar7;
  MATRIX *pMVar8;
  undefined1 *puVar9;
  undefined *puVar10;
  uint uVar11;
  undefined *puVar12;
  undefined4 uVar13;
  uint uVar14;
  uint uVar15;
  undefined1 *puVar16;
  undefined *puVar17;
  undefined1 auStack_e8 [144];
  undefined *local_58;
  undefined4 local_50;
  undefined *local_48;
  uint local_40;
  undefined1 *puVar6;
  
  puVar12 = DAT_800cbc18;
  puVar16 = auStack_e8;
  local_58 = &DAT_800c8018 + (uint)DAT_800ce5e0 * 0x3c00;
  local_50 = DAT_800ce338;
  local_40 = (uint)(*(ushort *)(DAT_800ce324 + DAT_800d4820 * 0x20 + 0x66) >> 7);
  local_48 = &DAT_800cc22c + (uint)DAT_800ce5e0 * 0x1000;
  SetRotMatrix((MATRIX *)&DAT_800dcba8);
  SetTransMatrix((MATRIX *)&DAT_800dcba8);
  puVar6 = &DAT_800dbb70;
  do {
    puVar5 = puVar6 + -0x7c;
    sVar1 = *(short *)(puVar6 + -0x46);
    sVar2 = *(short *)(puVar6 + -0x44);
    *(int *)(puVar16 + 0x54) = (int)*(short *)(puVar6 + -0x48);
    *(int *)(puVar16 + 0x58) = (int)sVar1;
    *(int *)(puVar16 + 0x5c) = (int)sVar2;
    uVar3 = *(ushort *)(puVar6 + -100);
    pbVar7 = (byte *)(*(int *)(puVar6 + -0xc) + (uint)(byte)puVar6[-0x5b] * 8);
    uVar11 = (uint)pbVar7[1];
    puVar17 = puVar12;
    if ((uVar3 & 0xa000) == 0xa000) {
      iVar4 = FUN_8002c820(puVar16 + 0x54,*(undefined4 *)(puVar16 + 0x98));
      if ((iVar4 != 0) && (uVar11 != 0)) {
        uVar13 = 0x2c;
        if ((uVar3 & 0x1000) != 0) {
          uVar13 = 0x2e;
        }
        uVar14 = (uint)pbVar7[3];
        uVar15 = (uint)*pbVar7;
        if ((uVar3 & 0x200) == 0) {
          if (*(int *)(puVar16 + 0xb0) != 0) {
            pMVar8 = (MATRIX *)&DAT_800dcba8;
            gte_SetRotMatrix((MATRIX *)&DAT_800dcba8);
            gte_SetTransMatrix(pMVar8);
            *(undefined4 *)(puVar16 + 0xb0) = 0;
          }
          if ((DAT_800cfbd8 & 0x10000) == 0) {
            puVar17 = puVar12 + uVar11 * 0x28;
            if (*(undefined **)(puVar16 + 0x90) <= puVar17) goto LAB_80077e50;
            *(undefined4 *)(puVar16 + 0x18) = uVar13;
            *(uint *)(puVar16 + 0x1c) = uVar15;
            *(undefined4 *)(puVar16 + 0x10) = *(undefined4 *)(puVar16 + 0xa8);
            *(undefined4 *)(puVar16 + 0x14) = *(undefined4 *)(puVar16 + 0xa0);
            FUN_80077ed0(puVar12,puVar5,uVar14,uVar11);
          }
          else {
            puVar17 = puVar12 + uVar11 * 0x28;
            puVar10 = puVar17 + uVar11 * 0x28;
            if (*(undefined **)(puVar16 + 0x90) <= puVar10) goto LAB_80077e50;
            *(undefined4 *)(puVar16 + 0x18) = uVar13;
            *(uint *)(puVar16 + 0x1c) = uVar15;
            *(undefined4 *)(puVar16 + 0x10) = *(undefined4 *)(puVar16 + 0xa8);
            *(undefined4 *)(puVar16 + 0x14) = *(undefined4 *)(puVar16 + 0xa0);
            FUN_80077ed0(puVar12,puVar5,uVar14,uVar11);
            *(int *)(puVar16 + 0x80) = (int)*(short *)(puVar5 + 0x34);
            *(int *)(puVar16 + 0x84) = (int)*(short *)(puVar5 + 0x36);
            *(int *)(puVar16 + 0x88) = (int)*(short *)(puVar5 + 0x38);
            iVar4 = FUN_8002ea40(*(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20,DAT_800cfb7d,
                                 puVar16 + 0x80);
            if (iVar4 != 0) {
              pMVar8 = (MATRIX *)&DAT_800e2a90;
              gte_SetRotMatrix((MATRIX *)&DAT_800e2a90);
              gte_SetTransMatrix(pMVar8);
              *(undefined4 *)(puVar16 + 0x18) = uVar13;
              *(uint *)(puVar16 + 0x1c) = uVar15;
              *(undefined4 *)(puVar16 + 0x10) = *(undefined4 *)(puVar16 + 0xa8);
              *(undefined4 *)(puVar16 + 0x14) = *(undefined4 *)(puVar16 + 0xa0);
              FUN_80077ed0(puVar17,puVar5,uVar14,uVar11);
              *(undefined4 *)(puVar16 + 0xb0) = 1;
              puVar17 = puVar10;
            }
          }
        }
        else {
          *(int *)(puVar16 + 0x54) =
               (int)*(short *)(puVar6 + -0x50) + (int)*(short *)(puVar6 + -0x58);
          *(int *)(puVar16 + 0x58) =
               (int)*(short *)(puVar6 + -0x4e) + (int)*(short *)(puVar6 + -0x56);
          *(int *)(puVar16 + 0x5c) =
               (int)*(short *)(puVar6 + -0x4c) + (int)*(short *)(puVar6 + -0x54);
          RotMatrix((SVECTOR *)(puVar6 + -0x38),(MATRIX *)(puVar16 + 0x40));
          puVar9 = puVar16 + 0x60;
          FUN_8002ce94(puVar6 + -0x30,(MATRIX *)(puVar16 + 0x40),puVar9);
          FUN_8002ce94(&DAT_800dcba8,puVar9,puVar16 + 0x20);
          pMVar8 = (MATRIX *)(puVar16 + 0x20);
          gte_SetRotMatrix(pMVar8);
          gte_SetTransMatrix(pMVar8);
          if ((DAT_800cfbd8 & 0x10000) == 0) {
            puVar17 = puVar12 + uVar11 * 0x28;
            if (*(undefined **)(puVar16 + 0x90) <= puVar17) goto LAB_80077e50;
            *(undefined4 *)(puVar16 + 0x18) = uVar13;
            *(uint *)(puVar16 + 0x1c) = uVar15;
            *(undefined4 *)(puVar16 + 0x10) = *(undefined4 *)(puVar16 + 0xa8);
            *(undefined4 *)(puVar16 + 0x14) = *(undefined4 *)(puVar16 + 0xa0);
            FUN_80078174(puVar12,puVar5,uVar14,uVar11);
          }
          else {
            puVar17 = puVar12 + uVar11 * 0x28;
            *(undefined **)(puVar16 + 0xb8) = puVar17 + uVar11 * 0x28;
            if (*(undefined **)(puVar16 + 0x90) <= puVar17 + uVar11 * 0x28) {
LAB_80077e50:
              *(undefined2 *)(puVar5 + 0x18) = 0;
              puVar17 = puVar12;
              goto LAB_80077e84;
            }
            *(undefined4 *)(puVar16 + 0x18) = uVar13;
            *(uint *)(puVar16 + 0x1c) = uVar15;
            *(undefined4 *)(puVar16 + 0x10) = *(undefined4 *)(puVar16 + 0xa8);
            *(undefined4 *)(puVar16 + 0x14) = *(undefined4 *)(puVar16 + 0xa0);
            FUN_80078174(puVar12,puVar5,uVar14,uVar11);
            *(int *)(puVar16 + 0x80) = (int)*(short *)(puVar5 + 0x34);
            *(int *)(puVar16 + 0x84) = (int)*(short *)(puVar5 + 0x36);
            *(int *)(puVar16 + 0x88) = (int)*(short *)(puVar5 + 0x38);
            iVar4 = FUN_8002ea40(*(int *)(DAT_800ce324 + 0x24) + DAT_800d4820 * 0x20,DAT_800cfb7d,
                                 puVar16 + 0x80);
            if (iVar4 != 0) {
              FUN_8002ce94(&DAT_800e2a90,puVar9,pMVar8);
              gte_SetRotMatrix(pMVar8);
              gte_SetTransMatrix(pMVar8);
              *(undefined4 *)(puVar16 + 0x18) = uVar13;
              *(uint *)(puVar16 + 0x1c) = uVar15;
              *(undefined4 *)(puVar16 + 0x10) = *(undefined4 *)(puVar16 + 0xa8);
              *(undefined4 *)(puVar16 + 0x14) = *(undefined4 *)(puVar16 + 0xa0);
              FUN_80078174(puVar17,puVar5,uVar14,uVar11);
              puVar17 = *(undefined **)(puVar16 + 0xb8);
            }
          }
          *(undefined4 *)(puVar16 + 0xb0) = 1;
        }
      }
    }
LAB_80077e84:
    puVar6 = puVar5;
    puVar12 = puVar17;
    if (puVar5 == &DAT_800d8cf0) {
      DAT_800cbc18 = puVar17;
      return;
    }
  } while( true );
}


