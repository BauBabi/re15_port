int rsin(int a)

{
  int iVar1;
  
  if (-1 < a) {
    sin_1(a & 0xfff);
    iVar1 = GEO_00_OBJ_2C();
    return iVar1;
  }
  iVar1 = sin_1(-a & 0xfff);
  return -iVar1;
}
