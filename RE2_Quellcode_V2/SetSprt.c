/* SetSprt @ 0x8008fae4  (Ghidra headless, raw MIPS overlay) */

void SetSprt(SPRT *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 4;
                    /* Probable PsyQ macro: setSprt() */
  p->code = 'd';
  return;
}


