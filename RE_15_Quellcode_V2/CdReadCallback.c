CdlCB CdReadCallback(CdlCB func)

{
  CdlCB pCVar1;
  
  pCVar1 = DAT_800787f4;
  DAT_800787f4 = func;
  return pCVar1;
}
