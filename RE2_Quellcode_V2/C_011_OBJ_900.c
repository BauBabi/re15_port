/* C_011_OBJ_900 @ 0x80089764  (Ghidra headless, raw MIPS overlay) */

void C_011_OBJ_900(void)

{
  *(undefined4 *)PTR_COMMON_DELAY_800ad160 = 0x1325;
  *DAT_800c3d68 = 3;
  DAT_800ea22c = DAT_800ea22c + 1;
  if ((DAT_800ead74 != 0) && (DAT_800dbb90 != 0)) {
    data_ready_callback();
  }
  return;
}


