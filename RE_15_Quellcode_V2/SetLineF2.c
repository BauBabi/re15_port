void SetLineF2(LINE_F2 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Possible PsyQ macro: setLineF2() */
  p->code = '@';
  return;
}
