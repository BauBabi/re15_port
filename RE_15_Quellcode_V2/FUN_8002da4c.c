bool FUN_8002da4c(int *param_1,int param_2,int param_3,char param_4)

{
  short *psVar1;
  
  if (*(char *)(param_2 + 0x82) == param_4) {
    psVar1 = *(short **)(param_2 + 0x78);
    if ((*param_1 - ((int)*psVar1 + *(int *)(param_2 + 0x34))) + (uint)(ushort)psVar1[3] + param_3
        <= ((uint)(ushort)psVar1[3] + param_3) * 2) {
      return (param_1[2] - ((int)psVar1[2] + *(int *)(param_2 + 0x3c))) +
             (uint)(ushort)psVar1[5] + param_3 <= ((uint)(ushort)psVar1[5] + param_3) * 2;
    }
  }
  return false;
}
