void Intpl(VECTOR *$2,long p,CVECTOR *$3)

{
  gte_ldlvl($2);
  gte_ldIR0(p);
  gte_intpl_b();
  gte_strgb($3);
  return;
}
