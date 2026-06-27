void DpqColorLight(VECTOR *$2,CVECTOR *$3,long p,CVECTOR *v2)

{
  gte_ldlvl($2);
  gte_ldrgb($3);
  gte_ldIR0(p);
  gte_dpcl_b();
  gte_strgb(v2);
  return;
}
