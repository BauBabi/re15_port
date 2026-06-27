/* FUN_800594c8 @ 0x800594c8  (Ghidra headless, raw MIPS overlay) */

void FUN_800594c8(int param_1,int param_2)

{
  if (param_2 != 0) {
    SsVabClose((short)(char)(&DAT_800d4c48)[param_1]);
    (&DAT_800d4c48)[param_1] = 0xff;
  }
  return;
}


