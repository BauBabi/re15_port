void FUN_8002f884(void)

{
  ushort uVar1;
  int iVar2;
  byte *pbVar3;
  undefined1 *puVar4;
  uint *puVar5;
  uint *puVar6;
  int iVar7;
  uint uVar8;
  ushort *puVar9;
  short sVar10;
  
  sVar10 = 0xbb;
  if ('\x05' < *(char *)(DAT_800b52d8 + 5)) {
    sVar10 = 0x25;
  }
  uVar8 = 0;
  puVar5 = *(uint **)(DAT_800b52d8 + 0x1e4);
  puVar9 = &DAT_80073b90;
  *(undefined2 *)((int)puVar5 + 0x12) = 0x30;
  *(undefined2 *)((int)puVar5 + 10) = 0x23;
  *(undefined2 *)(puVar5 + 4) = 0x60;
  *(short *)(puVar5 + 2) = sVar10;
  *(undefined1 *)(puVar5 + 3) = 0;
                    /* Possible PsyQ macro: setPolyG3() */
  *(undefined1 *)((int)puVar5 + 0xd) = 0x30;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar5 = *puVar5 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  puVar6 = puVar5 + 5;
  *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar5 & 0xffffff;
  *(undefined2 *)((int)puVar5 + 0x26) = 0x70;
  *(undefined2 *)((int)puVar5 + 0x1e) = 0x53;
  *(undefined2 *)(puVar5 + 9) = 0x60;
  *(short *)(puVar5 + 7) = sVar10;
  *(undefined1 *)(puVar5 + 8) = 0;
                    /* Possible PsyQ macro: setLineG2() */
  *(undefined1 *)((int)puVar5 + 0x21) = 0x50;
  iVar2 = DAT_800b52d8;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar6 = *puVar6 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar6 & 0xffffff;
  *(uint **)(iVar2 + 0x1e4) = puVar5 + 10;
  do {
    uVar1 = *puVar9;
    puVar9 = puVar9 + 1;
    pbVar3 = &DAT_80073d8c + uVar8;
    puVar4 = &DAT_80073d94 + uVar8;
    uVar8 = uVar8 + 1;
    FUN_80028ec4(sVar10 + (ushort)*pbVar3,*puVar4,&UNK_80073b18 + uVar1,1);
  } while (uVar8 < 8);
  if ((DAT_800ac762 & 0x1000) != 0) {
    *(char *)(DAT_800b52d8 + 6) = *(char *)(DAT_800b52d8 + 6) + -1;
  }
  if ((DAT_800ac762 & 0x4000) != 0) {
    *(char *)(DAT_800b52d8 + 6) = *(char *)(DAT_800b52d8 + 6) + '\x01';
  }
  if ('\a' < *(char *)(DAT_800b52d8 + 6)) {
    *(undefined1 *)(DAT_800b52d8 + 6) = 0;
  }
  if (*(char *)(DAT_800b52d8 + 6) < '\0') {
    *(undefined1 *)(DAT_800b52d8 + 6) = 7;
  }
  iVar2 = DAT_800b52d8;
  iVar7 = (uint)DAT_800aca34 * 0x30 + 0xc + DAT_800b52d8;
  *(undefined2 *)(iVar7 + 0x2c) = 0x48;
  *(undefined2 *)(iVar7 + 0x2e) = 0x10;
  *(short *)(iVar7 + 0x28) = sVar10 + 0xc;
  *(short *)(iVar7 + 0x2a) = *(char *)(iVar2 + 6) * 0x10 + 0x36;
                    /* Probable PsyQ macro: addPrim(). */
  *(uint *)(iVar7 + 0x20) =
       *(uint *)(iVar7 + 0x20) & 0xff000000 |
       *(uint *)(&DAT_800aa69c + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa69c + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa69c + (uint)DAT_800aca34 * 0x20) & 0xff000000 | iVar7 + 0x20U & 0xffffff;
  return;
}
