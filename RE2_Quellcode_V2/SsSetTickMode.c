/* SsSetTickMode @ 0x8007ee20  (Ghidra headless, raw MIPS overlay) */

void SsSetTickMode(long param_1)

{
  int iVar1;
  
  iVar1 = FUN_80086b7c();
  if ((param_1 & 0x1000U) != 0) {
    DAT_800aba2c = 1;
    DAT_800aba28 = param_1 & 0xfff;
    SSTICK_OBJ_50();
    return;
  }
  DAT_800aba2c = 0;
  DAT_800aba28 = param_1;
  if (5 < param_1) {
    DAT_800ea228 = param_1;
    return;
  }
  switch(param_1) {
  case 0:
    if ((iVar1 != 0) && (iVar1 == 1)) {
SSTICK_OBJ_12C:
      DAT_800ea228 = 0x32;
      SSTICK_OBJ_158();
      return;
    }
    break;
  case 1:
    DAT_800aba28 = 0x3c;
    DAT_800ea228 = 0x3c;
    if (iVar1 == 0) {
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
    if (iVar1 != 1) {
      DAT_800aba28 = 0x32;
      SSTICK_OBJ_158();
      return;
    }
SSTICK_OBJ_C8:
    SSTICK_OBJ_158();
    return;
  case 5:
    if (iVar1 != 0) {
      if (iVar1 != 1) {
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


