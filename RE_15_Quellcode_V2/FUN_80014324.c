int FUN_80014324(uint param_1)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(*(int *)(DAT_800ac778 + 0x28) + 2);
  iVar2 = *(int *)(DAT_800ac778 + 0x28);
  while (bVar1 != param_1) {
    bVar1 = *(byte *)(iVar2 + 0x16);
    iVar2 = iVar2 + 0x14;
  }
  return iVar2;
}
