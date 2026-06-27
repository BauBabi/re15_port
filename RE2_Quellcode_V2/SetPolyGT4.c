/* SetPolyGT4 @ 0x8008faa8  (Ghidra headless, raw MIPS overlay) */

void SetPolyGT4(POLY_GT4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 0xc;
                    /* Possible PsyQ macro: setPolyGT4() */
  p->code = '<';
  return;
}


