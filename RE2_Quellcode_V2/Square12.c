/* Square12 @ 0x8008dee4  (Ghidra headless, raw MIPS overlay) */

VECTOR * Square12(VECTOR *_2,VECTOR *_3)

{
  gte_ldlvl(_2);
  gte_sqr12_b();
  gte_stlvnl(_3);
  return _3;
}


