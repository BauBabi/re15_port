/* FUN_80052ca0 @ 0x80052ca0  (Ghidra headless, raw MIPS overlay) */

void FUN_80052ca0(void)

{
  int iVar1;
  
  if ((DAT_800cfb74 & 0x1000000) == 0) {
    iVar1 = (DAT_800d8cb8 + ((uint)(DAT_800d8cb8 << 1) >> 0x10) & 0xffff) +
            (DAT_800d8cb8 << 1 & 0xffff0000U);
    DAT_800d8cb8 = iVar1;
  }
  else {
    iVar1 = FUN_80015fe8();
  }
  DAT_800d4826 = (short)iVar1;
  return;
}


