void C_011_OBJ_900(void)

{
  COMMON_DELAY = 0x1325;
  *DAT_8008fe1c = 3;
  DAT_800bb4c4 = DAT_800bb4c4 + 1;
  if ((DAT_800bbd88 != 0) && (DAT_800b5220 != 0)) {
    data_ready_callback();
  }
  return;
}
