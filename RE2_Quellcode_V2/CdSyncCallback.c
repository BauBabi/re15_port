/* CdSyncCallback @ 0x80086e2c  (Ghidra headless, raw MIPS overlay) */

CdlCB CdSyncCallback(CdlCB func)

{
  CdlCB pCVar1;
  
  pCVar1 = DAT_800acdec;
  DAT_800acdec = func;
  return pCVar1;
}


