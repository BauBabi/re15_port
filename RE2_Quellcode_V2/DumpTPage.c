/* DumpTPage @ 0x8008f840  (Ghidra headless, raw MIPS overlay) */

void DumpTPage(u_short tpage)

{
  (*(code *)PTR_printf_800b26fc)
            ("tpage: (%d,%d,%d,%d)\n",tpage >> 7 & 3,tpage >> 5 & 3,(tpage & 0x1f) << 6,
             (tpage & 0x10) * 0x10 + (tpage >> 2 & 0x200));
  return;
}


