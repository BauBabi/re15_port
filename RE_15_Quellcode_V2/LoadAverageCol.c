void LoadAverageCol(u_char *$2,u_char *$3,long p0,long p1,u_char *v2)

{
  gte_ldIR0(p0);
  gte_ldsv_((uint)*$2,(uint)$2[1],(uint)$2[2]);
  gte_gpf0_b(0);
  gte_stLZCR();
  gte_ldIR0(p1);
  gte_ldsv_((uint)*$3,(uint)$3[1],(uint)$3[2]);
  gte_gpl0_b(0);
  read_mt((uint)*$3,(uint)$3[1],(uint)$3[2]);
  *v2 = '\0';
  v2[1] = '\0';
  v2[2] = '\0';
  return;
}
