void LoadAverage12(VECTOR *$2,VECTOR *$3,long p0,long p1,VECTOR *v2)

{
  undefined1 auStackX_0 [16];
  
  gte_ldIR0(p0);
  gte_ldlvl($2);
  gte_gpf12_b();
  gte_stLZCR();
  gte_ldIR0(p1);
  gte_ldlvl($3);
  gte_gpl12_b();
  gte_stlvl(*(VECTOR **)((int)register0x00000074 + 0x10));
  return;
}
