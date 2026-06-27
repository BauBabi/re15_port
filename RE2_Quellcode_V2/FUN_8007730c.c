/* FUN_8007730c @ 0x8007730c  (Ghidra headless, raw MIPS overlay) */

void FUN_8007730c(int param_1,uint param_2)

{
  uint *puVar1;
  
  puVar1 = (uint *)((param_2 >> 5) * 4 + param_1);
  *puVar1 = *puVar1 | 0x80000000U >> (param_2 & 0x1f);
  return;
}


