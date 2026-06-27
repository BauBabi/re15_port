/* FUN_80069adc @ 0x80069adc  (Ghidra headless, raw MIPS overlay) */

void FUN_80069adc(int param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_80069668(0);
  if ((param_1 == 0x13) || (5 < param_1 - 0xeU)) {
    FUN_8006947c(uVar1,param_1,param_2,0);
    iVar2 = (uVar1 >> 1) + uVar1;
  }
  else {
    FUN_800698b4(1);
    FUN_8006947c(0,param_1,param_2,1);
    FUN_80069bb4(0,param_3);
    FUN_8006947c(1,param_1,param_2,2);
    iVar2 = 1;
    param_3 = param_3 + 0x4b0;
  }
  FUN_80069bb4(iVar2,param_3);
  return;
}


