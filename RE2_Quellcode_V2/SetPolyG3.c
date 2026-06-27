/* SetPolyG3 @ 0x8008fa44  (Ghidra headless, raw MIPS overlay) */

void SetPolyG3(POLY_G3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 6;
                    /* Possible PsyQ macro: setPolyG3() */
  p->code = '0';
  return;
}


