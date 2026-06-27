/* sin_1 @ 0x8008cd00  (Ghidra headless, raw MIPS overlay) */

int sin_1(int param_1)

{
  int iVar1;
  
  if (0x800 < param_1) {
    if (0xc00 < param_1) {
      iVar1 = GEO_00_OBJ_C4();
      return iVar1;
    }
    return -(int)*(short *)(&DAT_800ac348 + param_1 * 2);
  }
  if (param_1 < 0x401) {
    iVar1 = GEO_00_OBJ_C4();
    return iVar1;
  }
  iVar1 = GEO_00_OBJ_C4();
  return iVar1;
}


