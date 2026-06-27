void SetTile(TILE *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Probable PsyQ macro: setTile() */
  p->code = '`';
  return;
}
