void SSTICK_OBJ_88(int param_1)

{
  DAT_800bb4bc = 0x32;
  if (param_1 != 1) {
    DAT_800776d4 = 0x32;
    SSTICK_OBJ_158();
    return;
  }
  DAT_800776d4 = 5;
  SSTICK_OBJ_158();
  return;
}
