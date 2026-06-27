VECTOR * Square12(VECTOR *$2,VECTOR *$3)

{
  gte_ldlvl($2);
  gte_sqr12_b();
  gte_stlvnl($3);
  return $3;
}
