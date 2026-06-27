/* FUN_80049ad0 @ 0x80049ad0  (Ghidra headless, raw MIPS overlay) */

void FUN_80049ad0(void)

{
  DAT_800ea240 = DAT_800ce32c;
  DAT_800ea244 = DAT_800ce32c + (uint)*(byte *)(DAT_800ce324 + 7) * 0x20;
  DAT_800ce32c = DAT_800ea244 + (uint)*(byte *)(DAT_800ce324 + 7) * 0x20;
  return;
}


