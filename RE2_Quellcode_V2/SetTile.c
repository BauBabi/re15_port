/* SetTile @ 0x8008fb34  (Ghidra headless, raw MIPS overlay) */

void SetTile(TILE *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 3;
                    /* Probable PsyQ macro: setTile() */
  p->code = '`';
  return;
}


