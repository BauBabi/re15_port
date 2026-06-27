/* GetODE @ 0x8009114c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int GetODE(void)

{
  uint uVar1;
  
  uVar1 = (**(code **)(PTR_PTR_800b26f8 + 0x38))();
  return uVar1 >> 0x1f;
}


