/* SetTile16 @ 0x8008fb20  (Ghidra headless, raw MIPS overlay) */

void SetTile16(TILE_16 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 2;
                    /* Possible PsyQ macro: setTile8() */
  p->code = 'x';
  return;
}


