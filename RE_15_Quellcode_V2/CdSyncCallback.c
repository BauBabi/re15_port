CdlCB CdSyncCallback(CdlCB func)

{
  CdlCB pCVar1;
  
  pCVar1 = DAT_800788a4;
  DAT_800788a4 = func;
  return pCVar1;
}
