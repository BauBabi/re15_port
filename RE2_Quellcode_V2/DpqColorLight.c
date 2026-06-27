/* DpqColorLight @ 0x8008de5c  (Ghidra headless, raw MIPS overlay) */

void DpqColorLight(VECTOR *_2,CVECTOR *_3,long p,CVECTOR *v2)

{
  gte_ldlvl(_2);
  gte_ldrgb(_3);
  gte_ldIR0(p);
  gte_dpcl_b();
  gte_strgb(v2);
  return;
}


