/* FUN_80016fe4 @ 0x80016fe4  (Ghidra headless, raw MIPS overlay) */

void FUN_80016fe4(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x1c) = *(uint *)(param_1 + 0x1c) & 0xff000000 | param_2;
  *(uint *)(param_1 + 0x44) = *(uint *)(param_1 + 0x44) & 0xff000000 | param_2;
  return;
}


