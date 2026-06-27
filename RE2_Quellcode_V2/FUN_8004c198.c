/* FUN_8004c198 @ 0x8004c198  (Ghidra headless, raw MIPS overlay) */

int FUN_8004c198(int param_1,int param_2,int param_3,int param_4)

{
  return (1 << ((uint)(param_1 - param_3) >> 0x1f)) << ((uint)(param_2 - param_4) >> 0x1e & 2);
}


