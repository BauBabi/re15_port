void _SsStart(int param_1)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0x3e6;
  do {
    bVar1 = -1 < iVar2;
    iVar2 = iVar2 + -1;
  } while (bVar1);
                    /* WARNING: Read-only address (ram,0x800776e4) is written */
  DAT_800776e4 = 0;
                    /* WARNING: Read-only address (ram,0x800776e6) is written */
  DAT_800776e6 = 6;
  DAT_800776e5 = 0;
  DAT_800776e0 = 0;
  if (DAT_800776d4 == 2) {
    SSSTART_OBJ_18C();
    return;
  }
  if (DAT_800776d4 < 3) {
    if (DAT_800776d4 != 0) {
      SSSTART_OBJ_E0();
      return;
    }
                    /* WARNING: Read-only address (ram,0x800776e6) is written */
    DAT_800776e6 = 0xff;
    SSSTART_OBJ_250();
    return;
  }
  if (DAT_800776d4 == 3) {
    SSSTART_OBJ_18C();
    return;
  }
  if (DAT_800776d4 != 5) {
    SSSTART_OBJ_E0();
    return;
  }
                    /* WARNING: Read-only address (ram,0x800776e6) is written */
  DAT_800776e6 = 0;
  if (param_1 == 0) {
                    /* WARNING: Read-only address (ram,0x800776e4) is written */
    DAT_800776e4 = 1;
    SSSTART_OBJ_18C();
    return;
  }
  SSSTART_OBJ_18C();
  return;
}
