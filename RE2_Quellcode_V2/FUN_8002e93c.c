/* FUN_8002e93c @ 0x8002e93c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e93c(undefined4 *param_1,undefined4 *param_2,char param_3,short param_4)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  uVar1 = param_1[1];
  uVar3 = param_1[2];
  uVar4 = param_1[3];
  *param_2 = *param_1;
  param_2[1] = uVar1;
  param_2[2] = uVar3;
  param_2[3] = uVar4;
  uVar1 = param_1[5];
  uVar3 = param_1[6];
  uVar4 = param_1[7];
  param_2[4] = param_1[4];
  param_2[5] = uVar1;
  param_2[6] = uVar3;
  param_2[7] = uVar4;
  iVar2 = (int)param_4;
  if (param_3 == '\x01') {
    param_2[1] = iVar2 * 2 - param_1[1];
    param_2[4] = iVar2 * 2 - param_1[4];
  }
  else if (param_3 == '\0') {
    param_2[3] = param_4 * 2 - param_1[3];
    param_2[6] = param_4 * 2 - param_1[6];
  }
  else {
    param_2[2] = iVar2 * 2 - param_1[2];
    param_2[5] = iVar2 * 2 - param_1[5];
  }
  return;
}


