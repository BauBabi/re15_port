void FUN_8002f3f0(void)

{
  byte bVar1;
  undefined2 *puVar2;
  uint *puVar3;
  
  puVar3 = (uint *)((uint)DAT_800aca34 * 0x30 + 0xc + DAT_800b52d8);
  bVar1 = *(byte *)(DAT_800b52d8 + 4);
  puVar2 = (undefined2 *)(*(char *)(DAT_800b52d8 + 5) * 4 + *(int *)(DAT_800b52d8 + 8));
  if (bVar1 == 2) {
    if (2 < *(char *)(DAT_800b52d8 + 5)) goto LAB_8002f490;
    *(undefined2 *)(puVar3 + 3) = 0x10;
  }
  else {
    if ((1 < bVar1) && (bVar1 < 5)) {
      *(undefined2 *)(puVar3 + 3) = 0x30;
      *(undefined2 *)((int)puVar3 + 0xe) = 0x10;
      *(undefined2 *)(puVar3 + 2) = 0xc6;
      *(undefined2 *)((int)puVar3 + 10) = 0x18;
      goto LAB_8002f4b4;
    }
LAB_8002f490:
    *(undefined2 *)(puVar3 + 3) = 0x30;
  }
  *(undefined2 *)((int)puVar3 + 0xe) = 0x10;
  *(undefined2 *)(puVar3 + 2) = *puVar2;
  *(undefined2 *)((int)puVar3 + 10) = puVar2[1];
LAB_8002f4b4:
                    /* Probable PsyQ macro: addPrim(). */
  *puVar3 = *puVar3 & 0xff000000 | *(uint *)(&DAT_800aa6ac + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa6ac + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6ac + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar3 & 0xffffff;
  return;
}
