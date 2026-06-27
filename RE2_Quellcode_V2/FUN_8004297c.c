/* FUN_8004297c @ 0x8004297c  (Ghidra headless, raw MIPS overlay) */

void FUN_8004297c(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x198) + 0xac0;
  if (((*(byte *)(param_1 + 8) & 1) != 0) && (*(byte *)(param_1 + 8) < 10)) {
    iVar1 = *(int *)(param_1 + 0x198) + 0xd70;
  }
  if (*(char *)(param_1 + 8) == '\x0e') {
    iVar1 = iVar1 + 0xac;
  }
  *(undefined1 *)(iVar1 + 0x7a) = 1;
  return;
}


