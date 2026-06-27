/* FUN_80046f98 @ 0x80046f98  (Ghidra headless, raw MIPS overlay) */

void FUN_80046f98(short *param_1,undefined2 *param_2)

{
  undefined2 uVar1;
  short sVar2;
  long lVar3;
  
  uVar1 = FUN_80047030((int)*param_1,(int)param_1[2]);
  lVar3 = SquareRoot0((int)*param_1 * (int)*param_1 + (int)param_1[2] * (int)param_1[2]);
  sVar2 = FUN_80047030(lVar3,-(int)param_1[1]);
  *param_2 = 0;
  param_2[1] = uVar1;
  param_2[2] = sVar2 - 0x400U & 0xfff;
  return;
}


