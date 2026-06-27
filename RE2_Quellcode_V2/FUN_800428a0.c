/* FUN_800428a0 @ 0x800428a0  (Ghidra headless, raw MIPS overlay) */

void FUN_800428a0(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x198) + 0xd70;
  if (*(char *)(param_1 + 8) == '\x0e') {
    iVar1 = *(int *)(param_1 + 0x198) + 0xb6c;
  }
  *(undefined2 *)(iVar1 + 0x84) = 0;
  *(undefined1 *)(iVar1 + 0x7a) = 0;
  *(undefined2 *)(iVar1 + 0x86) = 0;
  return;
}


