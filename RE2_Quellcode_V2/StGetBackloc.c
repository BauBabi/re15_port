/* StGetBackloc @ 0x80089a54  (Ghidra headless, raw MIPS overlay) */

int StGetBackloc(CdlLOC *loc)

{
  int iVar1;
  
  if (DAT_800d4c6c == 0) {
    iVar1 = CdPosToInt((CdlLOC *)&DAT_800c3d70);
    CdIntToPos(iVar1 + 1,loc);
    iVar1 = C_004_OBJ_DC();
    return iVar1;
  }
  return -1;
}


