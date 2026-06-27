void SetDrawMove(DR_MOVE *p)

{
  *(undefined1 *)((int)&p->tag + 3) = 5;
  *(undefined1 *)((int)&p->code + 3) = 1;
  p->code2 = 0x80000000;
  return;
}
