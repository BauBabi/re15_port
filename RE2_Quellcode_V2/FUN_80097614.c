/* FUN_80097614 @ 0x80097614  (Ghidra headless, raw MIPS overlay) */

void FUN_80097614(int param_1,undefined1 param_2)

{
  *(undefined1 *)(param_1 + 0x36) = 0x4c;
  *(int *)(param_1 + 0x2c) = param_1 + 0x24;
  *(undefined1 *)(param_1 + 0x24) = param_2;
  *(undefined1 *)(param_1 + 0x35) = 1;
  return;
}


