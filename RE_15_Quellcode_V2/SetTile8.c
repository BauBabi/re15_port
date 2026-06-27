void SetTile8(TILE_8 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 2;
                    /* Possible PsyQ macro: setTile16() */
  p->code = 'p';
  return;
}
