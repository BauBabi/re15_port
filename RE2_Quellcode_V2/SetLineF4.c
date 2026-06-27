/* SetLineF4 @ 0x8008fbb0  (Ghidra headless, raw MIPS overlay) */

void SetLineF4(LINE_F4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 6;
                    /* Possible PsyQ macro: setLineF4() */
  p->code = 'L';
  p->pad = 0x55555555;
  return;
}


