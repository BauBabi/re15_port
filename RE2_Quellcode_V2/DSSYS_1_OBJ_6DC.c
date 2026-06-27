/* DSSYS_1_OBJ_6DC @ 0x8008b544  (Ghidra headless, raw MIPS overlay) */

void DSSYS_1_OBJ_6DC(undefined4 param_1)

{
  DS_cw_root(param_1,0);
  if ((DAT_800c3f08 != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f08)();
  }
  if (((DAT_800ad254 == 1) && (DAT_800ad26f == '\0')) && (DAT_800ad260 == 0)) {
    DAT_800ad250 = 0x21;
    DS_cw_root(1,0);
  }
  return;
}


