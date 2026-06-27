void SetPolyFT4(POLY_FT4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 9;
                    /* Probable PsyQ macro: setPolyFT4() */
  p->code = ',';
  return;
}
