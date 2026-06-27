/* DsSystemStatus @ 0x8008ac04  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int DsSystemStatus(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = DS_system_status(0);
  if ((iVar1 == 1) && (iVar2 = DsQueueLen(), 0 < iVar2)) {
    iVar1 = 2;
  }
  return iVar1;
}


