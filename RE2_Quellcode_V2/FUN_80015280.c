/* FUN_80015280 @ 0x80015280  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80015280(int *param_1,short *param_2)

{
  undefined4 uVar1;
  
  uVar1 = 0;
  if (((uint)(*param_1 - (int)*param_2) <= (uint)(int)param_2[2]) &&
     (uVar1 = 0, (uint)(param_1[2] - (int)param_2[1]) <= (uint)(int)param_2[3])) {
    uVar1 = 1;
  }
  return uVar1;
}


