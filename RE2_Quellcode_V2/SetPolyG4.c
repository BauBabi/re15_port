/* SetPolyG4 @ 0x8008fa94  (Ghidra headless, raw MIPS overlay) */

void SetPolyG4(POLY_G4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 8;
                    /* Possible PsyQ macro: setPolyG4() */
  p->code = '8';
  return;
}


