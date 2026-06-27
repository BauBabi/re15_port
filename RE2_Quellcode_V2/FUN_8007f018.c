/* FUN_8007f018 @ 0x8007f018  (Ghidra headless, raw MIPS overlay) */

void FUN_8007f018(short param_1,undefined2 param_2,undefined2 param_3)

{
  int iVar1;
  
  iVar1 = (&DAT_800ea250)[param_1];
  if (*(int *)(iVar1 + 0x98) == 1) {
    FUN_800840ac((int)param_1,param_2,param_3,1);
  }
  else {
    *(undefined2 *)(iVar1 + 0x58) = param_2;
    *(undefined2 *)(iVar1 + 0x5a) = param_3;
  }
  return;
}


