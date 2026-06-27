/* CdReadyCallback @ 0x80086e44  (Ghidra headless, raw MIPS overlay) */

CdlCB CdReadyCallback(CdlCB func)

{
  CdlCB pCVar1;
  
  pCVar1 = DAT_800acdf0;
  DAT_800acdf0 = func;
  return pCVar1;
}


