/* Lzc @ 0x8008e028  (Ghidra headless, raw MIPS overlay) */

long Lzc(long data)

{
  long lVar1;
  
  gte_ldLZCS(data);
  lVar1 = gte_stLZCR();
  return lVar1;
}


