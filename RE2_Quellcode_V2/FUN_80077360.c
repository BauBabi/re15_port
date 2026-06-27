/* FUN_80077360 @ 0x80077360  (Ghidra headless, raw MIPS overlay) */

uint FUN_80077360(int param_1,uint param_2)

{
  return *(uint *)((param_2 >> 5) * 4 + param_1) & 0x80000000U >> (param_2 & 0x1f);
}


