/* FUN_80097f30 @ 0x80097f30  (Ghidra headless, raw MIPS overlay) */

void FUN_80097f30(int param_1)

{
  undefined1 uVar1;
  
  uVar1 = *(undefined1 *)(param_1 + 0x36);
  *(undefined1 *)(param_1 + 0x36) = 0;
  *(undefined1 *)(param_1 + 0x37) = uVar1;
  return;
}


