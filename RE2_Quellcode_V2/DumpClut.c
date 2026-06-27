/* DumpClut @ 0x8008f8a0  (Ghidra headless, raw MIPS overlay) */

void DumpClut(u_short clut)

{
  (*(code *)PTR_printf_800b26fc)("clut: (%d,%d)\n",(clut & 0x3f) << 4,clut >> 6);
  return;
}


