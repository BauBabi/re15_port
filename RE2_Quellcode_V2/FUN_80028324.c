/* FUN_80028324 @ 0x80028324  (Ghidra headless, raw MIPS overlay) */

int FUN_80028324(int param_1,int param_2)

{
  *(char *)(param_1 + 0x107) = (char)(*(uint *)(*(int *)(param_1 + 0x14) + -4) >> 1);
  *(int *)(param_1 + 0x198) = param_2;
  return param_2 + (uint)*(byte *)(param_1 + 0x107) * 0xac;
}


