/* printf @ 0x80098894  (Ghidra headless, raw MIPS overlay) */

int printf(char *fmt,...)

{
  int iVar1;
  
  iVar1 = (*(code *)&LAB_000000a0)();
  return iVar1;
}


