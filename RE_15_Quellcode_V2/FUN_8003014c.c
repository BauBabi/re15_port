void FUN_8003014c(void)

{
  byte bVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  
  puVar2 = (uint *)((uint)DAT_800aca34 * 0x40 + 0x6c + DAT_800b52d8);
  bVar1 = *(byte *)(DAT_800b52d8 + 4);
  if (bVar1 != 3) {
    if (bVar1 < 4) {
      if (bVar1 == 0) {
        return;
      }
      *(undefined2 *)(puVar2 + 3) = 0x140;
      *(undefined2 *)((int)puVar2 + 0xe) = 0xc0;
      *(undefined2 *)(puVar2 + 2) = 0;
      *(undefined2 *)((int)puVar2 + 10) = 0x30;
      goto LAB_80030338;
    }
    if (bVar1 != 4) {
      return;
    }
    *(undefined2 *)(puVar2 + 3) = 0x74;
    *(undefined2 *)((int)puVar2 + 0xe) = 0xc0;
    *(undefined2 *)(puVar2 + 2) = 0;
    *(undefined2 *)((int)puVar2 + 10) = 0x30;
                    /* Probable PsyQ macro: addPrim(). */
    *puVar2 = *puVar2 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff
    ;
    puVar3 = puVar2 + 4;
    *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
         *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar2 & 0xffffff
    ;
    *(undefined2 *)(puVar2 + 7) = 0x58;
    *(undefined2 *)((int)puVar2 + 0x1e) = 0x98;
    *(undefined2 *)(puVar2 + 6) = 0x74;
    *(undefined2 *)((int)puVar2 + 0x1a) = 0x58;
                    /* Probable PsyQ macro: addPrim(). */
    *puVar3 = *puVar3 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff
    ;
    puVar4 = puVar2 + 8;
    *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
         *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar3 & 0xffffff
    ;
    *(undefined2 *)(puVar2 + 0xb) = 0x74;
    *(undefined2 *)((int)puVar2 + 0x2e) = 0xc0;
    *(undefined2 *)(puVar2 + 10) = 0xcc;
    *(undefined2 *)((int)puVar2 + 0x2a) = 0x30;
                    /* Probable PsyQ macro: addPrim(). */
    *puVar4 = *puVar4 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff
    ;
    puVar2 = puVar2 + 0xc;
    *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
         *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar4 & 0xffffff
    ;
  }
  *(undefined2 *)(puVar2 + 3) = 0x140;
  *(undefined2 *)((int)puVar2 + 0xe) = 0x30;
  *(undefined2 *)(puVar2 + 2) = 0;
  *(undefined2 *)((int)puVar2 + 10) = 0;
LAB_80030338:
                    /* Probable PsyQ macro: addPrim(). */
  *puVar2 = *puVar2 & 0xff000000 | *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6a4 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar2 & 0xffffff;
  return;
}
