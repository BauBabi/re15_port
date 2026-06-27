void SetTile1(TILE_1 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 2;
                    /* Possible PsyQ macro: setTile1() */
  p->code = 'h';
  return;
}
