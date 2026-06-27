/* SetLineG3 @ 0x8008fb90  (Ghidra headless, raw MIPS overlay) */

void SetLineG3(LINE_G3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 7;
                    /* Possible PsyQ macro: setLineG3() */
  p->code = 'X';
  p->pad = 0x55555555;
  return;
}


