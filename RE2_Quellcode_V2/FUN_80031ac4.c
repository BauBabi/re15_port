/* FUN_80031ac4 @ 0x80031ac4  (Ghidra headless, raw MIPS overlay) */

void FUN_80031ac4(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = &DAT_800e8438;
  iVar1 = 0xc0;
  do {
    *puVar2 = 0;
    iVar1 = iVar1 + -1;
    puVar2 = puVar2 + 1;
  } while (iVar1 != 0);
  DAT_800e8738 = &DAT_800e8438;
  return;
}


