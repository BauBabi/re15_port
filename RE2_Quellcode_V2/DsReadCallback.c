/* DsReadCallback @ 0x8008c69c  (Ghidra headless, raw MIPS overlay) */

DslCB DsReadCallback(DslCB func)

{
  DslCB pDVar1;
  
  pDVar1 = DAT_800ad328;
  DAT_800ad328 = func;
  return pDVar1;
}


