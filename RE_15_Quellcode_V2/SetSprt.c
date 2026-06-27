void SetSprt(SPRT *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 4;
                    /* Probable PsyQ macro: setSprt() */
  p->code = 'd';
  return;
}
