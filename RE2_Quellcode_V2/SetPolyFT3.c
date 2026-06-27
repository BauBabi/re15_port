/* SetPolyFT3 @ 0x8008fa30  (Ghidra headless, raw MIPS overlay) */

void SetPolyFT3(POLY_FT3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 7;
                    /* Possible PsyQ macro: setPolyFT3() */
  p->code = '$';
  return;
}


