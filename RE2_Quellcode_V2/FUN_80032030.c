/* FUN_80032030 @ 0x80032030  (Ghidra headless, raw MIPS overlay) */

void FUN_80032030(int param_1)

{
  if ((&DAT_800d76a4)[param_1 * 0x40] != 0) {
    (&DAT_800d76a4)[param_1 * 0x40] = 0;
    FUN_800957a4();
    CloseTh(*(undefined4 *)(&DAT_800d76ac + param_1 * 0x80));
    FUN_800957b4();
  }
  return;
}


