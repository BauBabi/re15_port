void FUN_80019e20(void)

{
  ushort *puVar1;
  ushort uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  ushort *puVar5;
  char *pcVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  
  if ((DAT_800aca40 & 0x10000000) == 0) {
    DAT_800b52c4 = &DAT_800a73b8;
    do {
      if ((DAT_800b52c4[0x36] & 1) != 0) {
        (*(code *)(&PTR_LAB_80071d40)[*DAT_800b52c4])();
      }
      DAT_800b52c4 = DAT_800b52c4 + 0x42;
    } while (DAT_800b52c4 < &UNK_800aa538);
    DAT_800b52c4 = &DAT_800a73b8;
    do {
      if (((byte)DAT_800b52c4[0x36] & 8) != 0) {
        *(byte *)(DAT_800b52c4 + 0x36) = (byte)DAT_800b52c4[0x36] ^ 9;
        (*(code *)(&PTR_LAB_80071d40)[*DAT_800b52c4])();
      }
      puVar1 = DAT_800b52c4;
      if ((DAT_800b52c4[0x36] & 1) != 0) {
        if ((DAT_800b52c4[0x36] & 4) != 0) {
          puVar3 = *(undefined4 **)(DAT_800b52c4 + 0x3a);
          uVar4 = puVar3[1];
          uVar7 = puVar3[2];
          uVar8 = puVar3[3];
          *(undefined4 *)(DAT_800b52c4 + 0x26) = *puVar3;
          *(undefined4 *)(puVar1 + 0x28) = uVar4;
          *(undefined4 *)(puVar1 + 0x2a) = uVar7;
          *(undefined4 *)(puVar1 + 0x2c) = uVar8;
          uVar4 = puVar3[5];
          uVar7 = puVar3[6];
          uVar8 = puVar3[7];
          *(undefined4 *)(puVar1 + 0x2e) = puVar3[4];
          *(undefined4 *)(puVar1 + 0x30) = uVar4;
          *(undefined4 *)(puVar1 + 0x32) = uVar7;
          *(undefined4 *)(puVar1 + 0x34) = uVar8;
        }
        if ((DAT_800b52c4[0x36] & 0x80) == 0) {
          DAT_1f800000 = 0x1000;
          DAT_1f800004 = 0;
          DAT_1f800008 = 0x1000;
          DAT_1f80000c = 0;
          DAT_1f800010 = 0x1000;
          DAT_1f800014 = 0;
          DAT_1f800018 = 0;
          DAT_1f80001c = 0;
          DAT_1f80002c = DAT_800b52c4[0x10];
          DAT_1f80002e = DAT_800b52c4[0x11] + DAT_800b52c4[0x17];
          DAT_1f800030 = DAT_800b52c4[0x12];
          RotMatrix((SVECTOR *)&DAT_1f80002c,(MATRIX *)&DAT_1f800000);
          DAT_1f80002c = DAT_800b52c4[0x1a];
          DAT_1f80002e = DAT_800b52c4[0x1c];
          DAT_1f800030 = DAT_800b52c4[0x1e];
          ApplyMatrix((MATRIX *)&DAT_1f800000,(SVECTOR *)&DAT_1f80002c,(VECTOR *)&DAT_1f800020);
          puVar1 = DAT_800b52c4;
          DAT_800b52c4[0x14] = DAT_1f800020;
          DAT_1f80002c = puVar1[0x20];
          puVar1[0x15] = DAT_1f800024;
          puVar1[0x16] = DAT_1f800028;
          DAT_1f80002e = puVar1[0x22];
          DAT_1f800030 = puVar1[0x24];
          ApplyMatrix((MATRIX *)(puVar1 + 0x26),(SVECTOR *)&DAT_1f80002c,(VECTOR *)&DAT_1f800020);
          puVar5 = DAT_800b52c4;
          puVar1 = DAT_800b52c4 + 0x32;
          DAT_800b52c4[0x14] = DAT_800b52c4[0x14] + DAT_800b52c4[0x30] + DAT_1f800020;
          puVar5[0x15] = puVar5[0x15] + *puVar1 + DAT_1f800024;
          uVar2 = puVar5[0x16] + puVar5[0x34] + DAT_1f800028;
        }
        else {
          DAT_1f800000 = 0x1000;
          DAT_1f800004 = 0;
          DAT_1f800008 = 0x1000;
          DAT_1f80000c = 0;
          DAT_1f800010 = 0x1000;
          DAT_1f800014 = 0;
          DAT_1f800018 = 0;
          DAT_1f80001c = 0;
          RotMatrix((SVECTOR *)(DAT_800b52c4 + 0x10),(MATRIX *)&DAT_1f800000);
          DAT_1f80002c = DAT_800b52c4[0x1a];
          DAT_1f80002e = DAT_800b52c4[0x1c];
          DAT_1f800030 = DAT_800b52c4[0x1e];
          ApplyMatrix((MATRIX *)&DAT_1f800000,(SVECTOR *)&DAT_1f80002c,(VECTOR *)&DAT_1f800020);
          DAT_1f80002c = DAT_1f800020 + DAT_800b52c4[0x20];
          DAT_1f80002e = DAT_1f800024 + DAT_800b52c4[0x22];
          DAT_1f800030 = DAT_1f800028 + DAT_800b52c4[0x24];
          ApplyMatrix((MATRIX *)(DAT_800b52c4 + 0x26),(SVECTOR *)&DAT_1f80002c,
                      (VECTOR *)&DAT_1f800020);
          puVar5 = DAT_800b52c4;
          DAT_800b52c4[0x14] = DAT_800b52c4[0x30] + DAT_1f800020;
          puVar5[0x15] = puVar5[0x32] + DAT_1f800024;
          uVar2 = puVar5[0x34] + DAT_1f800028;
        }
        puVar5[0x16] = uVar2;
        (*(code *)(&PTR_LAB_80071d40)[DAT_800b52c4[1]])();
        puVar1 = DAT_800b52c4;
        if ((DAT_800b52c4[0x36] & 0x20) == 0) {
          puVar5 = DAT_800b52c4 + 0xd;
          uVar2 = DAT_800b52c4[0xe];
          DAT_800b52c4[0x10] = DAT_800b52c4[0x10] + DAT_800b52c4[0xc];
          puVar1[0x11] = puVar1[0x11] + *puVar5;
          puVar1[0x12] = puVar1[0x12] + uVar2;
          *(int *)(puVar1 + 0x1a) = (int)(short)puVar1[8] + *(int *)(puVar1 + 0x1a);
          *(int *)(puVar1 + 0x1c) = (int)(short)puVar1[9] + *(int *)(puVar1 + 0x1c);
          *(int *)(puVar1 + 0x1e) = (int)(short)puVar1[10] + *(int *)(puVar1 + 0x1e);
          puVar1[8] = puVar1[8] + puVar1[4];
          puVar1[9] = puVar1[9] + puVar1[5];
          puVar1[10] = puVar1[10] + puVar1[6];
        }
        if (*(char *)((int)DAT_800b52c4 + 0x6d) == '\0') {
          if ((DAT_800b52c4[0x36] & 0x40) == 0) {
            *(char *)(DAT_800b52c4 + 0x37) = (char)DAT_800b52c4[0x37] + '\x01';
          }
          pcVar6 = (char *)((uint)(byte)DAT_800b52c4[0x37] * 8 + *(int *)(DAT_800b52c4 + 0x3c));
          if ((pcVar6[2] == '\0') && (*pcVar6 == '\0')) {
            *(undefined1 *)(DAT_800b52c4 + 0x36) = 0;
          }
          else if (pcVar6[2] == -1) {
            *(char *)(DAT_800b52c4 + 0x37) = *pcVar6;
            pcVar6 = (char *)(*(int *)(DAT_800b52c4 + 0x3c) + (uint)(byte)DAT_800b52c4[0x37] * 8);
          }
          *(char *)((int)DAT_800b52c4 + 0x6d) = pcVar6[2];
        }
        *(char *)((int)DAT_800b52c4 + 0x6d) = *(char *)((int)DAT_800b52c4 + 0x6d) + -1;
      }
      DAT_800b52c4 = DAT_800b52c4 + 0x42;
    } while (DAT_800b52c4 < &UNK_800aa538);
  }
  return;
}
