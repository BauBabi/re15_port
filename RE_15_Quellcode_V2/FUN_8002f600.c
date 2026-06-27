void FUN_8002f600(void)

{
  int iVar1;
  uint *puVar2;
  uint *puVar3;
  
  puVar2 = *(uint **)(DAT_800b52d8 + 0x1e4);
  *(undefined2 *)(puVar2 + 4) = 0x32;
  *(undefined2 *)(puVar2 + 2) = 0x93;
  *(undefined2 *)((int)puVar2 + 0x12) = 0x18;
  *(undefined2 *)((int)puVar2 + 10) = 0x10;
  *(undefined1 *)(puVar2 + 3) = 0;
  *(undefined1 *)((int)puVar2 + 0xd) = 0xc0;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar2 = *puVar2 & 0xff000000 | *(uint *)(&DAT_800aa6b0 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  puVar3 = puVar2 + 5;
  *(uint *)(&DAT_800aa6b0 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6b0 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar2 & 0xffffff;
  *(undefined2 *)(puVar2 + 9) = 0x30;
  *(undefined2 *)(puVar2 + 7) = 0xc6;
  *(undefined2 *)((int)puVar2 + 0x26) = 0x10;
  *(undefined2 *)((int)puVar2 + 0x1e) = 0x18;
  *(undefined1 *)(puVar2 + 8) = 0;
  *(undefined1 *)((int)puVar2 + 0x21) = 0xd8;
  iVar1 = DAT_800b52d8;
                    /* Probable PsyQ macro: addPrim(). */
  *puVar3 = *puVar3 & 0xff000000 | *(uint *)(&DAT_800aa6b0 + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa6b0 + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6b0 + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar3 & 0xffffff;
  *(uint **)(iVar1 + 0x1e4) = puVar2 + 10;
  return;
}
