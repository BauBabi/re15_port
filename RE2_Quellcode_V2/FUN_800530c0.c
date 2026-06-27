/* FUN_800530c0 @ 0x800530c0  (Ghidra headless, raw MIPS overlay) */

void FUN_800530c0(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = &DAT_800ce558;
  iVar1 = 0x20;
  do {
    *puVar2 = 0;
    iVar1 = iVar1 + -1;
    puVar2 = puVar2 + 1;
  } while (iVar1 != 0);
  DAT_800d4230 = 0;
  return;
}


