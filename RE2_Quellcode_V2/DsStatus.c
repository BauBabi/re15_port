/* DsStatus @ 0x8008ac64  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

u_char DsStatus(void)

{
  u_char uVar1;
  
  uVar1 = DS_status();
  return uVar1;
}


