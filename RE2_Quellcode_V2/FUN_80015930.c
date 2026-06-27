/* FUN_80015930 @ 0x80015930  (Ghidra headless, raw MIPS overlay) */

bool FUN_80015930(int param_1,int param_2)

{
  short sVar1;
  
  sVar1 = FUN_800154ac((int)*(short *)(param_1 + 0x38),(int)*(short *)(param_1 + 0x40),
                       (int)*(short *)(param_2 + 0x38),(int)*(short *)(param_2 + 0x40));
  return (((int)*(short *)(param_2 + 0x76) - (int)sVar1) + 0x400U & 0xfff) < 0x800;
}


