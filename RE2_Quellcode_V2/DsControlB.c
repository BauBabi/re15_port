/* DsControlB @ 0x8008c1c4  (Ghidra headless, raw MIPS overlay) */

int DsControlB(u_char com,u_char *param,u_char *result)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DsCommand(com,param,(DslCB)0x0,0);
  if (iVar1 == 0) {
    iVar1 = DSSYS_4_OBJ_E8();
    return iVar1;
  }
  do {
    uVar2 = DsSync(iVar1,result);
  } while ((uVar2 & 0xff) == 0);
  return (uint)((uVar2 & 0xff) == 2);
}


