/* CdGetSector @ 0x80087224  (Ghidra headless, raw MIPS overlay) */

int CdGetSector(void *madr,int size)

{
  int iVar1;
  
  iVar1 = CD_getsector();
  return (uint)(iVar1 == 0);
}


