/* CdSetDebug @ 0x80086d6c  (Ghidra headless, raw MIPS overlay) */

int CdSetDebug(int level)

{
  int iVar1;
  
  iVar1 = DAT_800acdf8;
  DAT_800acdf8 = level;
  return iVar1;
}


