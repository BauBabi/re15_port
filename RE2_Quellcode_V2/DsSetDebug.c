/* DsSetDebug @ 0x8008c0c0  (Ghidra headless, raw MIPS overlay) */

int DsSetDebug(int level)

{
  int iVar1;
  
  iVar1 = DAT_800acdf8;
  DAT_800acdf8 = level;
  return iVar1;
}


