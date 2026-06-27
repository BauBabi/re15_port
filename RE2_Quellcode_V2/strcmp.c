/* strcmp @ 0x80098844  (Ghidra headless, raw MIPS overlay) */

int strcmp(char *param_1,char *param_2)

{
  int iVar1;
  
  iVar1 = (*(code *)&LAB_000000a0)();
  return iVar1;
}


