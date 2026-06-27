void SetPolyFT3(POLY_FT3 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 7;
                    /* Possible PsyQ macro: setPolyFT3() */
  p->code = '$';
  return;
}
