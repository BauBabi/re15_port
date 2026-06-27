void SetPolyGT4(POLY_GT4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 0xc;
                    /* Probable PsyQ macro: setPolyGT4() */
  p->code = '<';
  return;
}
