/* FUN_800363f0 @ 0x800363f0  (Ghidra headless, raw MIPS overlay) */

void FUN_800363f0(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  MATRIX *m1;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  MATRIX MStack_50;
  
  bVar1 = *(byte *)(DAT_800ce324 + 2);
  uVar6 = 0;
  if (bVar1 != 0) {
    puVar4 = &DAT_800d0324;
    do {
      uVar6 = uVar6 + 1;
      if (((*puVar4 & 1) != 0) && ((*puVar4 & 0x80000) == 0)) {
        RotMatrix((SVECTOR *)(puVar4 + 0x1d),(MATRIX *)(puVar4 + 9));
        m1 = (MATRIX *)(puVar4 + 0x15);
        FUN_8002ce94(puVar4[0x20],(MATRIX *)(puVar4 + 9),m1);
        if ((char)puVar4[2] == '\x02') {
          puVar4[0x1b] = puVar4[0x1b] - 900;
        }
        FUN_80076f88(puVar4 + 0x1a);
        SetColorMatrix((MATRIX *)&DAT_8009db84);
        MulMatrix0((MATRIX *)&DAT_8009db64,m1,&MStack_50);
        SetLightMatrix(&MStack_50);
        FUN_8002ce94(&DAT_800dcba8,m1,&MStack_50);
        SetRotMatrix(&MStack_50);
        SetTransMatrix(&MStack_50);
        uVar5 = puVar4[4] >> 1 & 1;
        if ((char)puVar4[2] == '\x01') {
          FUN_8002ef50(puVar4,uVar5);
        }
        else {
          iVar3 = FUN_8002c820(puVar4 + 0x1a,DAT_800ce338);
          if ((iVar3 != 0) || ((*puVar4 & 0x10) != 0)) {
            if ((puVar4[0x53] & 8) == 0) {
              if ((puVar4[4] & 0x10) == 0) {
                FUN_80027bec(puVar4[5],puVar4[6] + (uint)DAT_800ce5e0 * 0x28,puVar4[0x1f],uVar5);
                FUN_80027dbc(puVar4[7],puVar4[8] + (uint)DAT_800ce5e0 * 0x34,puVar4[0x1f],uVar5);
              }
              else {
                FUN_8002d0e8(puVar4[5],puVar4[6] + (uint)DAT_800ce5e0 * 0x28,puVar4[0x1f]);
                FUN_8002d244(puVar4[7],puVar4[8] + (uint)DAT_800ce5e0 * 0x34,puVar4[0x1f],uVar5);
              }
            }
            else {
              FUN_8002d3c8(puVar4[5],puVar4[6] + (uint)DAT_800ce5e0 * 0x28,puVar4 + 0x53);
              FUN_8002d718(puVar4[7],puVar4[8] + (uint)DAT_800ce5e0 * 0x34,puVar4 + 0x53);
              sVar2 = (short)puVar4[0x7b] + 1;
              *(short *)(puVar4 + 0x7b) = sVar2;
              if (sVar2 == 0x1e) {
                *puVar4 = 0;
              }
            }
          }
        }
        *(short *)(puVar4 + 0x11) = (short)puVar4[0xe];
        *(short *)((int)puVar4 + 0x46) = (short)puVar4[0xf];
        *(short *)(puVar4 + 0x12) = (short)puVar4[0x10];
        *(short *)((int)puVar4 + 0x4a) = (short)puVar4[0x21];
        *(short *)(puVar4 + 0x13) = (short)puVar4[0x22];
        *(short *)((int)puVar4 + 0x4e) = (short)puVar4[0x23];
      }
      puVar4 = puVar4 + 0x7e;
    } while (uVar6 < bVar1);
  }
  return;
}


