void SetPolyGT3(POLY_GT3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 9;
                    /* Possible PsyQ macro: setPolyGT3() */
  p->code = '4';
  return;
}
