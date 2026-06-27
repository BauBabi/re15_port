int StGetBackloc(CdlLOC *loc)

{
  int iVar1;
  
  if (DAT_800b2240 == 0) {
    iVar1 = CdPosToInt((CdlLOC *)&DAT_8008fe20);
    CdIntToPos(iVar1 + 1,loc);
    iVar1 = C_004_OBJ_DC();
    return iVar1;
  }
  return -1;
}
