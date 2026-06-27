void FUN_8002ac38(void)

{
  bool bVar1;
  int iVar2;
  
  bVar1 = -1 < iRam00000034;
  iVar2 = iRam00000034;
  while (bVar1) {
    FUN_8002acac(iVar2);
    iVar2 = iVar2 + -2;
    bVar1 = -1 < iVar2;
  }
  return;
}
