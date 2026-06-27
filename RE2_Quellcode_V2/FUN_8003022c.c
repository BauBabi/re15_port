/* FUN_8003022c @ 0x8003022c  (Ghidra headless, raw MIPS overlay) */

void FUN_8003022c(void)

{
  if ((DAT_800e873c & 0x80) != 0) {
    FUN_8003027c(&DAT_800e2ab0);
  }
  FUN_800316a8(&DAT_800e2ab0);
  FUN_80031ac4();
  return;
}


