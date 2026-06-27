/* DsSyncCallback @ 0x8008cc78  (Ghidra headless, raw MIPS overlay) */

DslCB DsSyncCallback(DslCB func)

{
  DslCB pDVar1;
  
  pDVar1 = DAT_800dcca8;
  DAT_800dcca8 = func;
  return pDVar1;
}


