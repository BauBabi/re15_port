/* SetLineF3 @ 0x8008fb70  (Ghidra headless, raw MIPS overlay) */

void SetLineF3(LINE_F3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 5;
                    /* Possible PsyQ macro: setLineF3() */
  p->code = 'H';
  p->pad = 0x55555555;
  return;
}


