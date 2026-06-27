/* SSTICK_OBJ_88 @ 0x8007eea8  (Ghidra headless, raw MIPS overlay) */

void SSTICK_OBJ_88(int param_1)

{
  DAT_800ea228 = 0x32;
  if (param_1 != 1) {
    DAT_800aba28 = 0x32;
    SSTICK_OBJ_158();
    return;
  }
  DAT_800aba28 = 5;
  SSTICK_OBJ_158();
  return;
}


