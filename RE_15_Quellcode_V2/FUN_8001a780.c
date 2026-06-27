bool FUN_8001a780(int param_1)

{
  return (((int)*(short *)(param_1 + 0x6a) - (int)*(short *)(DAT_800ac784 + 0x6a)) + 0x400U & 0xfff)
         < 0x800;
}
