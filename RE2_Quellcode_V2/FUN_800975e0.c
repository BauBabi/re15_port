/* FUN_800975e0 @ 0x800975e0  (Ghidra headless, raw MIPS overlay) */

void FUN_800975e0(int param_1,undefined1 param_2)

{
  *(undefined1 *)(param_1 + 0x36) = 0x43;
  *(int *)(param_1 + 0x2c) = param_1 + 0x24;
  *(undefined1 *)(param_1 + 0x24) = param_2;
  *(undefined1 *)(param_1 + 0x35) = 1;
  return;
}


