/* FUN_8009599c @ 0x8009599c  (Ghidra headless, raw MIPS overlay) */

bool FUN_8009599c(uint param_1)

{
  param_1 = param_1 & 0xffff;
  if (param_1 < 3) {
    *(undefined2 *)(PTR_TMR_DOTCLOCK_VAL_800c3978 + param_1 * 0x10) = 0;
  }
  return param_1 < 3;
}


