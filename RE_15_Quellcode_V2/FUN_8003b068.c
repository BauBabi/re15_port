uint FUN_8003b068(int *param_1,short *param_2,int param_3,int param_4)

{
  return (((int)param_2[2] + param_1[2]) - param_4 & 0x80000000U |
         (((int)*param_2 + *param_1) - param_3 & 0x80000000U) >> 1) >> 0x1e;
}
