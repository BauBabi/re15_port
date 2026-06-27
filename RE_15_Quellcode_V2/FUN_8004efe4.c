uint FUN_8004efe4(int param_1,uint param_2)

{
  return 0x80000000U >> (param_2 & 0x1f) & *(uint *)((param_2 >> 5) * 4 + param_1);
}
