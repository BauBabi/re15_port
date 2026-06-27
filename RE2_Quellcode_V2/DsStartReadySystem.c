/* DsStartReadySystem @ 0x8008c84c  (Ghidra headless, raw MIPS overlay) */

int DsStartReadySystem(DslRCB func,int count)

{
  int iVar1;
  
  if (DAT_800ad344 == 1) {
    iVar1 = DSREADY_OBJ_74();
    return iVar1;
  }
  DAT_800ad32c = 0xffffffff;
  DAT_800ad330 = 0;
  DAT_800ad338 = 0;
  DAT_800ad334 = func;
  DAT_800ad33c = count;
  DAT_800ad340 = DsReadyCallback(ER_cbready);
  DAT_800ad344 = 1;
  return (int)DAT_800ad340;
}


