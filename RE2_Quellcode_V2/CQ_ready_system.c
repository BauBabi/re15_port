/* CQ_ready_system @ 0x8008ad6c  (Ghidra headless, raw MIPS overlay) */

void CQ_ready_system(byte param_1,byte *param_2)

{
  if ((param_1 == 5) && ((*param_2 & 0x10) != 0)) {
    CQ_error_flush(5);
    DsEndReadySystem();
  }
  if (param_1 == 4) {
    DAT_800c3da8 = 1;
    DAT_800c3dac = param_1;
    rescpy(&DAT_800c3dad,param_2);
code_r0x8008ae34:
    if (DAT_800dccac != (code *)0x0) {
      (*DAT_800dccac)(param_1,param_2);
    }
    return;
  }
  if (param_1 < 5) {
    if (param_1 != 1) {
      DSSYS_2_OBJ_1380();
      return;
    }
  }
  else if (param_1 != 5) goto code_r0x8008ae34;
  DAT_800c3d98 = 1;
  DAT_800c3d9c = param_1;
  DSSYS_2_OBJ_1378(&DAT_800c3d9d,param_2);
  return;
}


