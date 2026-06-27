void LoadAverageByte(u_char *$2,u_char *$3,long p0,long p1,u_char *v2)

{
  int iVar1;
  int iVar2;
  
  gte_ldIR0(p0);
  gte_ldIR1((uint)*$2);
  gte_ldIR2((uint)$2[1]);
  gte_gpf0_b(0);
  gte_stLZCR();
  gte_ldIR0(p1);
  gte_ldIR1((uint)*$3);
  gte_ldIR2((uint)$3[1]);
  gte_gpl0_b(0);
  iVar1 = gte_stMAC1();
  iVar2 = gte_stMAC2();
  *v2 = (u_char)(iVar1 >> 0xc);
  v2[1] = (u_char)(iVar2 >> 0xc);
  return;
}
