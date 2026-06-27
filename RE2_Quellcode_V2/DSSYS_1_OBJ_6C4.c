/* DSSYS_1_OBJ_6C4 @ 0x8008b52c  (Ghidra headless, raw MIPS overlay) */

void DSSYS_1_OBJ_6C4(void)

{
  int in_v0;
  
  if (in_v0 == 0) {
    DAT_800ad250 = 0x20;
    DS_cw_root(1,0);
  }
  if ((DAT_800c3f08 != (code *)0x0) && (DAT_800ad238 != 0)) {
    (*DAT_800c3f08)();
  }
  if (((DAT_800ad254 == 1) && (DAT_800ad26f == '\0')) && (DAT_800ad260 == 0)) {
    DAT_800ad250 = 0x21;
    DS_cw_root(1,0);
  }
  return;
}


