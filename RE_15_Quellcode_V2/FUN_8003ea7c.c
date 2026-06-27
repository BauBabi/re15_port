void FUN_8003ea7c(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined1 *puVar4;
  undefined1 *puVar5;
  
  uVar3 = 0;
  iVar2 = 0;
  DAT_800ac9a0 = 0;
  DAT_800ac9a4 = 0;
  do {
    *(undefined4 *)((int)&DAT_800b3f98 + iVar2) = 0;
    uVar3 = uVar3 + 1;
    iVar2 = iVar2 + 0x94;
  } while (uVar3 < 0x20);
  uVar3 = 0;
  if (*(char *)(DAT_800ac778 + 2) != '\0') {
    puVar4 = &DAT_800b4019;
    puVar5 = &DAT_800b4018;
    iVar2 = -0x94;
    do {
      *puVar5 = DAT_800aca4c;
      *puVar4 = DAT_800aca4d;
      iVar1 = *(int *)(uVar3 * 8 + *(int *)(DAT_800ac778 + 0x30));
      if (DAT_800ac9a0 == iVar1) {
        *puVar5 = (&DAT_800b4018)[iVar2];
        *puVar4 = (&DAT_800b4019)[iVar2];
      }
      else {
        DAT_800ac9a0 = iVar1;
        if (*(int *)(uVar3 * 8 + *(int *)(DAT_800ac778 + 0x30)) != DAT_800ac778) {
          FUN_8004ee78();
        }
      }
      puVar5 = puVar5 + 0x94;
      puVar4 = puVar4 + 0x94;
      uVar3 = uVar3 + 1;
      iVar2 = iVar2 + 0x94;
    } while (uVar3 < *(byte *)(DAT_800ac778 + 2));
  }
  return;
}
