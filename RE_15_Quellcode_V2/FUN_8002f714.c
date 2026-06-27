void FUN_8002f714(void)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  
  cVar2 = *(char *)(DAT_800b52d8 + 6);
  if (cVar2 == '\a') {
    (&DAT_80073d08)[*(char *)(DAT_800b52d8 + 5)] = 7;
  }
  else {
    iVar3 = 0xb;
    do {
      if (cVar2 == (&DAT_80073d08)[iVar3]) {
        (&DAT_80073d08)[iVar3] = (&DAT_80073d08)[*(char *)(DAT_800b52d8 + 5)];
      }
      bVar1 = iVar3 != 0;
      iVar3 = iVar3 + -1;
    } while (bVar1);
    (&DAT_80073d08)[*(char *)(DAT_800b52d8 + 5)] = cVar2;
  }
  return;
}
