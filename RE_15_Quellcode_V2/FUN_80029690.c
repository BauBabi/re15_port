void FUN_80029690(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined *puVar4;
  
  iVar3 = 8;
  puVar4 = &UNK_800b835c;
  iVar2 = 0xc0;
  do {
    iVar2 = iVar2 + -0x18;
    iVar3 = iVar3 + -1;
    iVar1 = iVar3 * 4;
                    /* Probable PsyQ macro: addPrim(). */
    *(uint *)(&DAT_800b829c + (uint)DAT_800aca34 * 0xc0 + iVar2) =
         *(uint *)(&DAT_800b829c + (uint)DAT_800aca34 * 0xc0 + iVar2) & 0xff000000 |
         *(uint *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + iVar1) & 0xffffff;
    puVar4 = puVar4 + -0x18;
    *(uint *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + iVar1) =
         *(uint *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + iVar1) & 0xff000000 |
         (uint)(puVar4 + (uint)DAT_800aca34 * 0xc0) & 0xffffff;
                    /* Probable PsyQ macro: addPrim(). */
    *(uint *)(&DAT_800b82a8 + iVar2 + (uint)DAT_800aca34 * 0xc0) =
         *(uint *)(&DAT_800b82a8 + iVar2 + (uint)DAT_800aca34 * 0xc0) & 0xff000000 |
         *(uint *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + iVar1) & 0xffffff;
    *(uint *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + iVar1) =
         *(uint *)(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + iVar1) & 0xff000000 |
         (uint)(&DAT_800b82a8 + iVar2 + (uint)DAT_800aca34 * 0xc0) & 0xffffff;
  } while (iVar3 != 0);
  return;
}
