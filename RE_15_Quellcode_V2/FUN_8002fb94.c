void FUN_8002fb94(void)

{
  char cVar1;
  int iVar2;
  byte *pbVar3;
  short sVar4;
  int iVar5;
  uint *puVar6;
  uint *puVar7;
  undefined *puVar8;
  uint uVar9;
  undefined **ppuVar10;
  
  uVar9 = 0;
  ppuVar10 = &PTR_DAT_80073cc0;
  puVar6 = *(uint **)(DAT_800b52d8 + 0x1e4);
  *(undefined2 *)(puVar6 + 4) = 0x20;
  *(undefined2 *)((int)puVar6 + 0x12) = 0x80;
  *(undefined2 *)(puVar6 + 2) = 0xbb;
  *(undefined2 *)((int)puVar6 + 10) = 0x23;
  *(undefined1 *)(puVar6 + 3) = 0x60;
  *(undefined1 *)((int)puVar6 + 0xd) = 0;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar6 = *puVar6 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  puVar7 = puVar6 + 5;
  *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar6 & 0xffffff;
  *(undefined2 *)(puVar6 + 9) = 0x18;
  *(undefined2 *)(puVar6 + 7) = 0xdb;
  *(undefined2 *)((int)puVar6 + 0x26) = 0x80;
  *(undefined2 *)((int)puVar6 + 0x1e) = 0x23;
  *(undefined1 *)(puVar6 + 8) = 0x60;
  *(undefined1 *)((int)puVar6 + 0x21) = 0x80;
  iVar2 = DAT_800b52d8;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar7 = *puVar7 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar7 & 0xffffff;
  *(uint **)(iVar2 + 0x1e4) = puVar6 + 10;
  FUN_80028ec4(200,0x37,&DAT_80073b80,1);
  FUN_80028ec4(200,0x47,&DAT_80073b84,1);
  FUN_80028ec4(200,0x57,&DAT_80073b88,1);
  FUN_80028ec4(200,0x67,&DAT_80073b8c,1);
  FUN_800279c8(0x7700c8,0x81,&DAT_800109f4,0);
  FUN_800279c8(0x8700c8,0x81,&DAT_800109f0,0);
  do {
    puVar8 = *ppuVar10;
    ppuVar10 = ppuVar10 + 1;
    pbVar3 = &DAT_80073d94 + uVar9;
    uVar9 = uVar9 + 1;
    FUN_800279c8((uint)*pbVar3 << 0x10 | 0xd8,0x81,puVar8,0);
  } while (uVar9 < 6);
  if ((DAT_800ac762 & 0x1000) == 0) {
    if ((DAT_800ac762 & 0x4000) == 0) {
      if ((DAT_800ac762 & 0xa000) != 0) {
        cVar1 = *(char *)(DAT_800b52d8 + 6);
        if ((cVar1 < '\x06') && (-1 < cVar1)) {
          *(char *)(DAT_800b52d8 + 6) = cVar1 + '\x06';
        }
        else {
          *(char *)(DAT_800b52d8 + 6) = *(char *)(DAT_800b52d8 + 6) + -6;
        }
      }
    }
    else {
      cVar1 = *(char *)(DAT_800b52d8 + 6);
      if ((cVar1 == '\x05') || (cVar1 == '\v')) {
        *(char *)(DAT_800b52d8 + 6) = cVar1 + -5;
      }
      else {
        *(char *)(DAT_800b52d8 + 6) = cVar1 + '\x01';
      }
    }
  }
  else {
    cVar1 = *(char *)(DAT_800b52d8 + 6);
    if ((cVar1 == '\0') || (cVar1 == '\x06')) {
      *(char *)(DAT_800b52d8 + 6) = cVar1 + '\x05';
    }
    else {
      *(char *)(DAT_800b52d8 + 6) = cVar1 + -1;
    }
  }
  iVar2 = DAT_800b52d8;
  iVar5 = (uint)DAT_800aca34 * 0x30 + 0xc + DAT_800b52d8;
  *(undefined2 *)(iVar5 + 0x2c) = 0x10;
  *(undefined2 *)(iVar5 + 0x2e) = 0x10;
  puVar6 = (uint *)(iVar5 + 0x20);
  if (*(char *)(iVar2 + 6) < '\x06') {
    *(undefined2 *)(iVar5 + 0x28) = 199;
    sVar4 = *(char *)(iVar2 + 6) * 0x10 + 0x36;
  }
  else {
    *(undefined2 *)(iVar5 + 0x28) = 0xd7;
    sVar4 = *(char *)(iVar2 + 6) * 0x10 + -0x2a;
  }
  *(short *)(iVar5 + 0x2a) = sVar4;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar6 = *puVar6 & 0xff000000 | *(uint *)(&DAT_800aa69c + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa69c + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa69c + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar6 & 0xffffff;
  return;
}
