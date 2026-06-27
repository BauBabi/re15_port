/* Intpl @ 0x8008dec0  (Ghidra headless, raw MIPS overlay) */

void Intpl(VECTOR *_2,long p,CVECTOR *_3)

{
  gte_ldlvl(_2);
  gte_ldIR0(p);
  gte_intpl_b();
  gte_strgb(_3);
  return;
}


