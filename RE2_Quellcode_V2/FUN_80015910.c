/* FUN_80015910 @ 0x80015910  (Ghidra headless, raw MIPS overlay) */

bool FUN_80015910(int param_1,int param_2)

{
  return (((int)*(short *)(param_2 + 0x76) - (int)*(short *)(param_1 + 0x76)) + 0x400U & 0xfff) <
         0x800;
}


