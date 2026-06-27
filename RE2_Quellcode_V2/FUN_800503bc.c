/* FUN_800503bc @ 0x800503bc  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800503bc(int param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = param_6 * (param_1 - param_3);
  iVar3 = iVar2 / param_5;
  if (param_5 == 0) {
    trap(0x1c00);
  }
  if ((param_5 == -1) && (iVar2 == -0x80000000)) {
    trap(0x1800);
  }
  uVar1 = 0;
  if (iVar3 < param_2 - param_4) {
    uVar1 = 2;
    if (-iVar3 < param_2 - (param_4 + param_6)) {
      uVar1 = 3;
    }
  }
  else if (param_2 - (param_4 + param_6) < -iVar3) {
    uVar1 = 1;
  }
  return uVar1;
}


