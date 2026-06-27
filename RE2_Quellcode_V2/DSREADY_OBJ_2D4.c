/* DSREADY_OBJ_2D4 @ 0x8008cb20  (Ghidra headless, raw MIPS overlay) */

void DSREADY_OBJ_2D4(void)

{
  if (DAT_800ad338 == 1) {
    if ((DAT_800ad33c < 1) && (DAT_800ad33c != -1)) {
      if (DAT_800ad334 != (code *)0x0) {
        (*DAT_800ad334)(5);
      }
    }
    else {
      ER_retry();
      if (0 < DAT_800ad33c) {
        DAT_800ad33c = DAT_800ad33c + -1;
        DSREADY_OBJ_350();
        return;
      }
    }
    DAT_800ad338 = 0;
  }
  return;
}


