void FUN_800436a8(void)

{
  uint uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  FUN_80042bac(&DAT_800aca54,1,0);
  uVar1 = 0;
  if (DAT_800aca4e != 0) {
    puVar3 = &DAT_800acc2c;
    iVar2 = 0;
    do {
      if ((*(uint *)((int)&DAT_800acc2c + iVar2) & 1) != 0) {
        *(undefined1 *)((int)&DAT_800acc36 + iVar2 + 1) = 0;
        FUN_80042bac(puVar3,2,0);
        uVar1 = uVar1 + 1;
      }
      puVar3 = puVar3 + 0x7d;
      iVar2 = iVar2 + 500;
    } while (uVar1 < DAT_800aca4e);
  }
  uVar1 = 0;
  if (*(char *)(DAT_800ac778 + 2) != '\0') {
    puVar3 = &DAT_800b3f98;
    iVar2 = 0;
    do {
      *(undefined1 *)((int)&DAT_800b3fa2 + iVar2 + 1) = 0;
      FUN_80042bac(puVar3,4,0);
      puVar3 = puVar3 + 0x25;
      uVar1 = uVar1 + 1;
      iVar2 = iVar2 + 0x94;
    } while (uVar1 < *(byte *)(DAT_800ac778 + 2));
  }
  return;
}
