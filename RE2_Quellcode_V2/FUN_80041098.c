/* FUN_80041098 @ 0x80041098  (Ghidra headless, raw MIPS overlay) */

void FUN_80041098(int param_1,int param_2)

{
  if (DAT_800cfbf3 != '\0') {
    *(undefined1 *)(param_1 + 0x1ed) = 0;
    *(undefined1 *)(param_1 + 0x1ec) = *(undefined1 *)(*(int *)(param_2 + 4) + 1);
  }
  return;
}


