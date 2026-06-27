/* IsEndPrim @ 0x8008f8fc  (Ghidra headless, raw MIPS overlay) */

int IsEndPrim(void *p)

{
                    /* WARNING: Load size is inaccurate */
  return (uint)((*p & 0xffffff) == 0xffffff);
}


