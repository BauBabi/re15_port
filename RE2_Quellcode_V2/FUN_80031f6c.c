/* FUN_80031f6c @ 0x80031f6c  (Ghidra headless, raw MIPS overlay) */

void FUN_80031f6c(int param_1,undefined4 param_2)

{
  *(undefined4 *)(&DAT_800d76a8 + param_1 * 0x80) = param_2;
  (&DAT_800d76a4)[param_1 * 0x40] = 2;
  return;
}


