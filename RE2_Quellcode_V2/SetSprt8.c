/* SetSprt8 @ 0x8008fabc  (Ghidra headless, raw MIPS overlay) */

void SetSprt8(SPRT_8 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Possible PsyQ macro: setSprt8() */
  p->code = 't';
  return;
}


