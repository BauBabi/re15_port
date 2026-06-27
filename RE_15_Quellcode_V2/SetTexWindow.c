void SetTexWindow(DR_TWIN *p,RECT *tw)

{
  u_long uVar1;
  
  *(undefined1 *)((int)&p->tag + 3) = 2;
  uVar1 = get_tw(tw);
  p->code[0] = uVar1;
  p->code[1] = 0;
  return;
}
