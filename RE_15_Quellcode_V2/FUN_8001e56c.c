int FUN_8001e56c(int param_1,int param_2)

{
  *(char *)(param_1 + 0x83) = (char)(*(uint *)(*(int *)(param_1 + 0x10) + -4) >> 1);
  *(int *)(param_1 + 0x188) = param_2;
  return param_2 + (uint)*(byte *)(param_1 + 0x83) * 0xac;
}
