void SSTICK_OBJ_100(int param_1)

{
  if (param_1 == 0) {
    DAT_800bb4bc = 0x3c;
    SSTICK_OBJ_158();
    return;
  }
  if (param_1 != 1) {
    SSTICK_OBJ_140();
    return;
  }
  DAT_800bb4bc = 0x32;
  SSTICK_OBJ_158();
  return;
}
