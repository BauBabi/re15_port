/* FUN_80010778 @ 0x80010778  (Ghidra headless, raw MIPS overlay) */

void FUN_80010778(undefined4 *param_1,undefined4 *param_2,int param_3)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  puVar1 = (undefined4 *)((int)param_2 + param_3);
  do {
    uVar2 = param_2[1];
    uVar3 = param_2[2];
    uVar4 = param_2[3];
    *param_1 = *param_2;
    param_2 = param_2 + 4;
    param_1[1] = uVar2;
    param_1[2] = uVar3;
    param_1[3] = uVar4;
    param_1 = param_1 + 4;
  } while (param_2 < puVar1);
  return;
}


