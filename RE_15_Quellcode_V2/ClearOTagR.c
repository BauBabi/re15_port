u_long * ClearOTagR(u_long *ot,int n)

{
  if (1 < DAT_8007e352) {
    printf("ClearOTagR(%08x,%d)...\n",ot,n);
  }
  _otc(ot,n);
  *ot = 0x7e40c;
  return ot;
}
