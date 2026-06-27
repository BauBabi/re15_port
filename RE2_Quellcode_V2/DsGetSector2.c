/* DsGetSector2 @ 0x8008befc  (Ghidra headless, raw MIPS overlay) */

int DsGetSector2(void *madr,int size)

{
  int iVar1;
  
  iVar1 = CD_getsector2();
  return (uint)(iVar1 == 0);
}


