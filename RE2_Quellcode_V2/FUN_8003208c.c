/* FUN_8003208c @ 0x8003208c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003208c(undefined4 param_1)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_800d7824;
  *DAT_800d7824 = 2;
  *(undefined4 *)(puVar1 + 2) = param_1;
  FUN_800957a4();
  CloseTh(*(undefined4 *)(puVar1 + 4));
  FUN_800957b4();
  ChangeTh(0xff000000);
  return;
}


