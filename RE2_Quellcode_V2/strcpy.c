/* strcpy @ 0x80098854  (Ghidra headless, raw MIPS overlay) */

char * strcpy(char *param_1,char *param_2)

{
  char *pcVar1;
  
  pcVar1 = (char *)(*(code *)&LAB_000000a0)();
  return pcVar1;
}


