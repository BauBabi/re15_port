void C_011_OBJ_7BC(void)

{
  if ((ushort)DAT_8008fe1c[3] - 1 != (uint)(ushort)DAT_8008fe1c[2]) {
    if (DAT_800bbd88 != 0) {
      mem2mem(DAT_800bbda0,DAT_800b5350 * 0x800 + DAT_800bbd88 + 0x20,0x1f8,0);
      DAT_800b5350 = DAT_800b5350 + 1;
      C_011_OBJ_900();
      return;
    }
    dma_execute(3,DAT_800bbda0,0,0x1f8);
    COMMON_DELAY = 0x1325;
    *DAT_8008fe1c = 3;
    DAT_800bb4c4 = DAT_800bb4c4 + 1;
    if ((DAT_800bbd88 != 0) && (DAT_800b5220 != 0)) {
      data_ready_callback();
    }
    return;
  }
  DAT_800b5220 = 1;
  if (DAT_800bbd88 != 0) {
    mem2mem(DAT_800bbda0,DAT_800b5350 * 0x800 + DAT_800bbd88 + 0x20,0x1f8,1);
    DAT_800b5350 = DAT_800b5350 + 1;
    C_011_OBJ_85C();
    return;
  }
  dma_execute(3,DAT_800bbda0,0,0x1f8);
  DAT_800b2208 = 0;
  DAT_800aa5e0 = 0;
  DAT_800b2b40 = DAT_800b266c;
  C_011_OBJ_900();
  return;
}
