/* SSTICK_OBJ_50 @ 0x8007ee70  (Ghidra headless, raw MIPS overlay) */

void SSTICK_OBJ_50(int param_1)

{
  if (5 < DAT_800aba28) {
    DAT_800ea228 = DAT_800aba28;
    return;
  }
  switch(DAT_800aba28) {
  case 0:
    if ((param_1 != 0) && (param_1 == 1)) {
SSTICK_OBJ_12C:
      DAT_800ea228 = 0x32;
      SSTICK_OBJ_158();
      return;
    }
    break;
  case 1:
    DAT_800aba28 = 0x3c;
    DAT_800ea228 = 0x3c;
    if (param_1 == 0) {
      DAT_800aba28 = 5;
    }
    goto SSTICK_OBJ_C8;
  case 2:
    DAT_800ea228 = 0xf0;
    SSTICK_OBJ_158();
    return;
  case 3:
    DAT_800ea228 = 0x78;
    SSTICK_OBJ_158();
    return;
  case 4:
    DAT_800ea228 = 0x32;
    DAT_800aba28 = 5;
    if (param_1 != 1) {
      DAT_800aba28 = 0x32;
      SSTICK_OBJ_158();
      return;
    }
SSTICK_OBJ_C8:
    SSTICK_OBJ_158();
    return;
  case 5:
    if (param_1 != 0) {
      if (param_1 != 1) {
        SSTICK_OBJ_140();
        return;
      }
      goto SSTICK_OBJ_12C;
    }
  }
  DAT_800ea228 = 0x3c;
  SSTICK_OBJ_158();
  return;
}


