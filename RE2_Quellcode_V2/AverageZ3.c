/* AverageZ3 @ 0x8008df34  (Ghidra headless, raw MIPS overlay) */

long AverageZ3(long sz0,long sz1,long sz2)

{
  long lVar1;
  
  gte_ldsz3(sz0,sz1,sz2);
  gte_avsz3_b();
  lVar1 = gte_stOTZ();
  return lVar1;
}


