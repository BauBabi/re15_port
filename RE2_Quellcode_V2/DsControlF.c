/* DsControlF @ 0x8008c130  (Ghidra headless, raw MIPS overlay) */

int DsControlF(u_char com,u_char *param)

{
  int iVar1;
  
  iVar1 = DsCommand(com,param,(DslCB)0x0,0);
  return iVar1;
}


