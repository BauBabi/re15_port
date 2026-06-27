/* FUN_8004a3c0 @ 0x8004a3c0  (Ghidra headless, raw MIPS overlay) */

void FUN_8004a3c0(void)

{
  int iVar1;
  
  iVar1 = FUN_80031e80(2,DAT_800d481c + 8);
  DAT_800ce328 = &DAT_8011a000 + (iVar1 + 3U & 0xfffffffc);
  FUN_80031f94(1);
  return;
}


