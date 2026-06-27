void SetSprt8(SPRT_8 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Probable PsyQ macro: setSprt8() */
  p->code = 't';
  return;
}
