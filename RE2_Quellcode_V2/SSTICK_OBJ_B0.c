/* SSTICK_OBJ_B0 @ 0x8007eed0  (Ghidra headless, raw MIPS overlay) */

void SSTICK_OBJ_B0(int param_1)

{
  DAT_800aba28 = 0x3c;
  DAT_800ea228 = 0x3c;
  if (param_1 == 0) {
    DAT_800aba28 = 5;
  }
  SSTICK_OBJ_158();
  return;
}


