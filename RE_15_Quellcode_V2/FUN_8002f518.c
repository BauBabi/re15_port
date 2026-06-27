void FUN_8002f518(void)

{
  undefined2 *puVar1;
  int iVar2;
  uint *puVar3;
  
  iVar2 = (uint)DAT_800aca34 * 0x30 + 0xc + DAT_800b52d8;
  puVar1 = (undefined2 *)(*(char *)(DAT_800b52d8 + 5) * 4 + *(int *)(DAT_800b52d8 + 8));
  puVar3 = (uint *)(iVar2 + 0x10);
  if ((*(char *)(DAT_800b52d8 + 4) == '\x03') && (0xb < *(char *)(DAT_800b52d8 + 5))) {
    *(undefined2 *)(iVar2 + 0x1c) = 0x10;
    *(undefined2 *)(iVar2 + 0x1e) = 0x10;
  }
  else {
    *(undefined2 *)(iVar2 + 0x1c) = 0x48;
    *(undefined2 *)(iVar2 + 0x1e) = 0x10;
  }
  *(undefined2 *)(iVar2 + 0x18) = *puVar1;
  *(undefined2 *)(iVar2 + 0x1a) = puVar1[1];
                    /* Probable PsyQ macro: addPrim(). */
  *puVar3 = *puVar3 & 0xff000000 | *(uint *)(&DAT_800aa6ac + (uint)DAT_800aca34 * 0x20) & 0xffffff;
  *(uint *)(&DAT_800aa6ac + (uint)DAT_800aca34 * 0x20) =
       *(uint *)(&DAT_800aa6ac + (uint)DAT_800aca34 * 0x20) & 0xff000000 | (uint)puVar3 & 0xffffff;
  return;
}
