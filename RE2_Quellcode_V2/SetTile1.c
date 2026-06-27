/* SetTile1 @ 0x8008faf8  (Ghidra headless, raw MIPS overlay) */

void SetTile1(TILE_1 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 2;
                    /* Possible PsyQ macro: setTile1() */
  p->code = 'h';
  return;
}


