/* FUN_80049a9c @ 0x80049a9c  (Ghidra headless, raw MIPS overlay) */

void FUN_80049a9c(void)

{
  DAT_800eaad0 = DAT_800ce32c;
  DAT_800ce32c = DAT_800ce32c + (uint)*(byte *)(DAT_800ce324 + 7) * 4;
  return;
}


