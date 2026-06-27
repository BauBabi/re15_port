/* AddPrim @ 0x8008f918  (Ghidra headless, raw MIPS overlay) */

void AddPrim(void *ot,void *p)

{
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
                    /* Probable PsyQ macro: addPrim(). */
  *(uint *)p = *p & 0xff000000 | *ot & 0xffffff;
                    /* WARNING: Load size is inaccurate */
  *(uint *)ot = *ot & 0xff000000 | (uint)p & 0xffffff;
  return;
}


