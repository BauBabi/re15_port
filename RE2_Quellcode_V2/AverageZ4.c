/* AverageZ4 @ 0x8008df54  (Ghidra headless, raw MIPS overlay) */

long AverageZ4(long sz0,long sz1,long sz2,long sz3)

{
  long lVar1;
  
  gte_ldsz4(sz0,sz1,sz2,sz3);
  gte_avsz4_b();
  lVar1 = gte_stOTZ();
  return lVar1;
}


