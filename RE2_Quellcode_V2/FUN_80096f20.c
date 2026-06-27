/* FUN_80096f20 @ 0x80096f20  (Ghidra headless, raw MIPS overlay) */

int FUN_80096f20(int param_1)

{
  return ((int)(*(byte *)(param_1 + 0xe3) + 1) >> 1) * 4 +
         ((uint)*(byte *)(param_1 + 0xe9) * 5 + 3 & 0xffc) + 4 + *(int *)(param_1 + 0xec);
}


