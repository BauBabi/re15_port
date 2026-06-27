/* DsReadyCallback @ 0x8008cc8c  (Ghidra headless, raw MIPS overlay) */

DslCB DsReadyCallback(DslCB func)

{
  DslCB pDVar1;
  
  pDVar1 = DAT_800dccac;
  DAT_800dccac = func;
  return pDVar1;
}


