/* StopCallback @ 0x80086178  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int StopCallback(void)

{
  int iVar1;
  
  iVar1 = (**(code **)(PTR_PTR_800acccc + 0x10))();
  return iVar1;
}


