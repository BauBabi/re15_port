/* FUN_80019024 @ 0x80019024  (Ghidra headless, raw MIPS overlay) */

void FUN_80019024(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_80019628(param_1,*(undefined4 *)(param_1 + 0x198),&DAT_8009a8e8,param_2);
  uVar1 = FUN_80019628(param_1,*(undefined4 *)(param_1 + 0x198),&DAT_8009a97c,uVar1);
  uVar1 = FUN_80019628(param_1,*(undefined4 *)(param_1 + 0x198),&DAT_8009aa10,uVar1);
  uVar1 = FUN_80019628(param_1,*(undefined4 *)(param_1 + 0x198),&DAT_8009aad8,uVar1);
  FUN_80019628(param_1,*(undefined4 *)(param_1 + 0x198),&DAT_8009ab58,uVar1);
  return;
}


