/* FUN_800845e8 @ 0x800845e8  (Ghidra headless, raw MIPS overlay) */

int FUN_800845e8(uint param_1,undefined2 *param_2,undefined2 *param_3)

{
  int iVar1;
  
  DAT_800dcc46 = (short)param_1;
  iVar1 = (&DAT_800ea250)[param_1 & 0xff] + ((int)(param_1 & 0xff00) >> 8) * 0xb0;
  *param_2 = *(undefined2 *)(iVar1 + 0x58);
  *param_3 = *(undefined2 *)(iVar1 + 0x5a);
  return (int)DAT_800dcc46;
}


