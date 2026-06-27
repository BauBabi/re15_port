void SetLineG2(LINE_G2 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 4;
                    /* Possible PsyQ macro: setLineG2() */
  p->code = 'P';
  return;
}
