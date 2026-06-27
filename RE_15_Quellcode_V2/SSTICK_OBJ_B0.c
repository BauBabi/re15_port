void SSTICK_OBJ_B0(int param_1)

{
  DAT_800776d4 = 0x3c;
  DAT_800bb4bc = 0x3c;
  if (param_1 == 0) {
    DAT_800776d4 = 5;
  }
  SSTICK_OBJ_158();
  return;
}
