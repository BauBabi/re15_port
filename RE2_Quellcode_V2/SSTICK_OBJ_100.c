/* SSTICK_OBJ_100 @ 0x8007ef20  (Ghidra headless, raw MIPS overlay) */

void SSTICK_OBJ_100(int param_1)

{
  if (param_1 == 0) {
    DAT_800ea228 = 0x3c;
    SSTICK_OBJ_158();
    return;
  }
  if (param_1 != 1) {
    SSTICK_OBJ_140();
    return;
  }
  DAT_800ea228 = 0x32;
  SSTICK_OBJ_158();
  return;
}


