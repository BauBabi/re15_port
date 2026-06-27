int rcos(int a)

{
  int iVar1;
  uint uVar2;
  
  if (a < 0) {
    a = -a;
  }
  uVar2 = a & 0xfff;
  if (uVar2 < 0x801) {
    if (uVar2 < 0x401) {
      iVar1 = GEO_01_OBJ_98();
      return iVar1;
    }
    iVar1 = GEO_01_OBJ_98();
    return iVar1;
  }
  if (0xc00 < uVar2) {
    iVar1 = GEO_01_OBJ_98();
    return iVar1;
  }
  return -(int)*(short *)(&DAT_8007db04 + (0xc00 - uVar2) * 2);
}
