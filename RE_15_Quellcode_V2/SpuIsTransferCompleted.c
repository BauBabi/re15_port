long SpuIsTransferCompleted(long flag)

{
  bool bVar1;
  long lVar2;
  int iVar3;
  
  if ((DAT_80076dc4 != 1) && (DAT_80076d8c != 1)) {
    iVar3 = TestEvent(DAT_80076e18);
    if (flag == 1) {
      bVar1 = iVar3 == 0;
      iVar3 = 1;
      if (bVar1) {
        do {
          iVar3 = TestEvent(DAT_80076e18);
        } while (iVar3 == 0);
        lVar2 = S_ITC_OBJ_88();
        return lVar2;
      }
    }
    else if (iVar3 != 1) {
      return iVar3;
    }
    DAT_80076d8c = iVar3;
    return iVar3;
  }
  lVar2 = S_ITC_OBJ_90();
  return lVar2;
}
