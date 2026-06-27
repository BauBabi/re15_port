/* FUN_80031fe4 @ 0x80031fe4  (Ghidra headless, raw MIPS overlay) */

void FUN_80031fe4(void)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_800d7824;
  *DAT_800d7824 = 0;
  FUN_800957a4();
  CloseTh(*(undefined4 *)(puVar1 + 4));
  FUN_800957b4();
  ChangeTh(0xff000000);
  return;
}


