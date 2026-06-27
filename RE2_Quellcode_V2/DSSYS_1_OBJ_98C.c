/* DSSYS_1_OBJ_98C @ 0x8008b7f4  (Ghidra headless, raw MIPS overlay) */

void DSSYS_1_OBJ_98C(void)

{
  DAT_800ad268 = DAT_800ad268 & 0xffff0000 | (DAT_800ad240 << 0x18 | DAT_800ad23c >> 8) >> 0x10;
  DAT_800ad264 = DAT_800ad264 & 0xffff | (DAT_800ad23c >> 8) << 0x10;
  DSSYS_1_OBJ_A10();
  return;
}


