/* TermPrim @ 0x8008f9b4  (Ghidra headless, raw MIPS overlay) */

void TermPrim(void *p)

{
                    /* WARNING: Load size is inaccurate */
  *(uint *)p = *p | 0xffffff;
  return;
}


