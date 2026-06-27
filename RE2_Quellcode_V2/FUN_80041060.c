/* FUN_80041060 @ 0x80041060  (Ghidra headless, raw MIPS overlay) */

void FUN_80041060(int param_1)

{
  *(char *)(param_1 + 0x107) =
       *(char *)(param_1 + 0x107) -
       (char)(*(uint *)(*(int *)(*(int *)(param_1 + 0x198) + 0x76c) + -4) >> 1);
  return;
}


