/* CdGetSector2 @ 0x80087244  (Ghidra headless, raw MIPS overlay) */

int CdGetSector2(void *madr,int size)

{
  int iVar1;
  
  iVar1 = CD_getsector2();
  return (uint)(iVar1 == 0);
}


