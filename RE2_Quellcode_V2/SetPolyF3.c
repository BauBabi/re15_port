/* SetPolyF3 @ 0x8008fa1c  (Ghidra headless, raw MIPS overlay) */

void SetPolyF3(POLY_F3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 4;
                    /* Possible PsyQ macro: setPolyF3() */
  p->code = ' ';
  return;
}


