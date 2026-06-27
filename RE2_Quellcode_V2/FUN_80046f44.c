/* FUN_80046f44 @ 0x80046f44  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80046f44(int param_1,int param_2,int param_3,short *param_4)

{
  undefined4 uVar1;
  short sVar2;
  uint uVar3;
  
  uVar3 = param_3 + (param_1 - param_2) & 0xfff;
  uVar1 = 0;
  if ((int)uVar3 < (param_3 << 0x10) >> 0xf) {
    *param_4 = (short)param_1;
  }
  else {
    sVar2 = (short)param_2 - (short)param_3;
    if (uVar3 < 0x801) {
      sVar2 = sVar2 + (short)param_3 * 2;
    }
    *param_4 = sVar2;
    uVar1 = 1;
  }
  return uVar1;
}


