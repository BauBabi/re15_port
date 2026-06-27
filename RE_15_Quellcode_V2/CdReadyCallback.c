CdlCB CdReadyCallback(CdlCB func)

{
  CdlCB pCVar1;
  
  pCVar1 = DAT_800788a8;
  DAT_800788a8 = func;
  return pCVar1;
}
