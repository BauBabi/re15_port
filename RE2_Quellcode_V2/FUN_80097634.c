/* FUN_80097634 @ 0x80097634  (Ghidra headless, raw MIPS overlay) */

void FUN_80097634(int param_1,undefined1 param_2)

{
  *(undefined1 *)(param_1 + 0x36) = 0x46;
  *(int *)(param_1 + 0x2c) = param_1 + 0x24;
  *(undefined1 *)(param_1 + 0x24) = param_2;
  *(undefined1 *)(param_1 + 0x35) = 1;
  return;
}


