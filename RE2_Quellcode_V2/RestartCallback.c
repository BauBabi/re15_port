/* RestartCallback @ 0x800861a8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int RestartCallback(void)

{
  int iVar1;
  
  iVar1 = (**(code **)(PTR_PTR_800acccc + 0x18))();
  return iVar1;
}


