int CdInit(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 4;
  do {
    iVar1 = CdReset(1);
    iVar2 = iVar2 + -1;
    if (iVar1 == 1) {
      CdSyncCallback(def_cbsync);
      CdReadyCallback(def_cbready);
      CdReadCallback(def_cbread);
      iVar2 = EVENT_OBJ_7C();
      return iVar2;
    }
  } while (iVar2 != -1);
  printf("CdInit: Init failed\n");
  return 0;
}
