/* DsGetSector @ 0x8008bedc  (Ghidra headless, raw MIPS overlay) */

int DsGetSector(void *madr,int size)

{
  int iVar1;
  
  iVar1 = CD_getsector();
  return (uint)(iVar1 == 0);
}


