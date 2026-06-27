/* SetPolyFT4 @ 0x8008fa80  (Ghidra headless, raw MIPS overlay) */

void SetPolyFT4(POLY_FT4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 9;
                    /* Probable PsyQ macro: setPolyFT4() */
  p->code = ',';
  return;
}


