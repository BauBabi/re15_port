void SetLineG4(LINE_G4 *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 9;
                    /* Possible PsyQ macro: setLineG4() */
  p->code = '\\';
  p->pad = 0x55555555;
  return;
}
