/* FUN_800783b4 @ 0x800783b4  (Ghidra headless, raw MIPS overlay) */

void FUN_800783b4(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = &DAT_800c4418;
  iVar2 = 0x300;
  do {
    *puVar1 = 0x9000000;
    puVar1[1] = 0x2c808080;
    iVar2 = iVar2 + -1;
    puVar1 = puVar1 + 10;
  } while (iVar2 != 0);
  FUN_800778f8();
  return;
}


