/* SetLineF2 @ 0x8008fb48  (Ghidra headless, raw MIPS overlay) */

void SetLineF2(LINE_F2 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Probable PsyQ macro: setLineF2() */
  p->code = '@';
  return;
}


