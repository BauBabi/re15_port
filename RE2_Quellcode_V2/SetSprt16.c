/* SetSprt16 @ 0x8008fad0  (Ghidra headless, raw MIPS overlay) */

void SetSprt16(SPRT_16 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Possible PsyQ macro: setSprt16() */
  p->code = '|';
  return;
}


