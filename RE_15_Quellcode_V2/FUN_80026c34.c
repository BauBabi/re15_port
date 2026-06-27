void FUN_80026c34(undefined1 *param_1)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  
  puVar1 = param_1;
  do {
    *puVar1 = 5;
    puVar1 = puVar1 + 1;
  } while (puVar1 < param_1 + 5);
  uVar2 = 0;
  iVar3 = 0;
  do {
    if (DAT_801ff556 <= uVar2) {
      return;
    }
    param_1[(byte)(&DAT_801ff418)[iVar3] - 0x30] = (char)uVar2;
    uVar2 = uVar2 + 1;
    iVar3 = iVar3 + 0x16;
  } while (uVar2 < 5);
                    /* WARNING: Read-only address (ram,0x801ff556) is written */
  return;
}
