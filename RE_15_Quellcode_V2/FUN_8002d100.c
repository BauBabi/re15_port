uint * FUN_8002d100(int param_1,undefined4 param_2)

{
  int iVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  
  iVar4 = 0x7fff;
  iVar3 = *(byte *)(DAT_800ac778 + 2) - 1;
  puVar5 = (uint *)0x0;
  if (-1 < iVar3) {
    puVar2 = &DAT_800b3f98 + iVar3 * 0x25;
    do {
      iVar3 = iVar3 + -1;
      if ((((*puVar2 & 1) != 0) &&
          (iVar1 = FUN_8002da4c(param_1 + 0x34,puVar2,param_2,*(undefined1 *)((int)puVar2 + 0x82)),
          iVar1 != 0)) && (*(int *)(param_1 + 0x38) < iVar4)) {
        iVar4 = *(int *)(param_1 + 0x38);
        puVar5 = puVar2;
      }
      puVar2 = puVar2 + -0x25;
    } while (-1 < iVar3);
  }
  return puVar5;
}
