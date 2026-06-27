/* SetPolyF4 @ 0x8008fa6c  (Ghidra headless, raw MIPS overlay) */

void SetPolyF4(POLY_F4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 5;
                    /* Probable PsyQ macro: setPolyF4() */
  p->code = '(';
  return;
}


