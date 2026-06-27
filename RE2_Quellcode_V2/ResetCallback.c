/* ResetCallback @ 0x80086084  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int ResetCallback(void)

{
  int iVar1;
  
  iVar1 = (**(code **)(PTR_PTR_800acccc + 0xc))();
  return iVar1;
}


