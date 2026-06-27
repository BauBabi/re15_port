/* C_011_OBJ_7BC @ 0x80089620  (Ghidra headless, raw MIPS overlay) */

void C_011_OBJ_7BC(void)

{
  if ((ushort)DAT_800c3d68[3] - 1 != (uint)(ushort)DAT_800c3d68[2]) {
    if (DAT_800ead74 != 0) {
      mem2mem(DAT_800ead98,DAT_800ead74 + DAT_800df33c * 0x800 + 0x20,0x1f8,0);
      DAT_800df33c = DAT_800df33c + 1;
      C_011_OBJ_900();
      return;
    }
    dma_execute(3,DAT_800ead98,0,0x1f8);
    *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1325;
    *DAT_800c3d68 = 3;
    DAT_800ea22c = DAT_800ea22c + 1;
    if ((DAT_800ead74 != 0) && (DAT_800dbb90 != 0)) {
      data_ready_callback();
    }
    return;
  }
  DAT_800dbb90 = 1;
  if (DAT_800ead74 != 0) {
    mem2mem(DAT_800ead98,DAT_800ead74 + DAT_800df33c * 0x800 + 0x20,0x1f8,1);
    DAT_800df33c = DAT_800df33c + 1;
    C_011_OBJ_85C();
    return;
  }
  dma_execute(3,DAT_800ead98,0,0x1f8);
  DAT_800d4c64 = 0;
  DAT_800cbc24 = 0;
  DAT_800d7850 = DAT_800d6c38;
  C_011_OBJ_900();
  return;
}


