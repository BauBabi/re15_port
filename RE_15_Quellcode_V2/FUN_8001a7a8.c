bool FUN_8001a7a8(int param_1,short param_2,short param_3)

{
  short sVar1;
  
  sVar1 = FUN_8001a6d4((int)param_2,(int)param_3,(int)*(short *)(param_1 + 0x34),
                       (int)*(short *)(param_1 + 0x3c));
  return (((int)sVar1 - (int)*(short *)(param_1 + 0x6a)) + 0x400U & 0xfff) < 0x800;
}
