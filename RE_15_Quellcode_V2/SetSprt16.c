void SetSprt16(SPRT_16 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Probable PsyQ macro: setSprt16() */
  p->code = '|';
  return;
}
