/* DsReadSync @ 0x8008c628  (Ghidra headless, raw MIPS overlay) */

int DsReadSync(u_char *result)

{
  int iVar1;
  int iVar2;
  
  iVar1 = VSync(-1);
  iVar2 = DAT_800ad30c;
  if (DAT_800ad31c + 0x4b0 < iVar1) {
    DsReadBreak();
    iVar2 = DSREAD_OBJ_448();
    return iVar2;
  }
  DsReady(result);
  return iVar2;
}


