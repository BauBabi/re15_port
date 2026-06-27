/* FUN_80031f94 @ 0x80031f94  (Ghidra headless, raw MIPS overlay) */

void FUN_80031f94(undefined2 param_1)

{
  undefined2 *puVar1;
  undefined4 unaff_retaddr;
  
  puVar1 = DAT_800d7824;
  DAT_800d7824[1] = param_1;
  *puVar1 = 1;
  DAT_800c3ab8 = unaff_retaddr;
  *(undefined4 *)(puVar1 + 0x28) = unaff_retaddr;
  ChangeTh(0xff000000);
  return;
}


