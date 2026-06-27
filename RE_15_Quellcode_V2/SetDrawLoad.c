void SetDrawLoad(DR_LOAD *p,RECT *rect)

{
  int iVar1;
  char cVar2;
  
  iVar1 = ((int)rect->w * (int)rect->h + 1) / 2;
  cVar2 = (char)iVar1 + '\x04';
  if (10 < iVar1 - 1U) {
    cVar2 = '\0';
  }
  p->code[0] = 0x1000000;
  *(char *)((int)&p->tag + 3) = cVar2;
  p->code[1] = 0xa0000000;
  p->code[2] = *(u_long *)rect;
  p->code[3] = *(u_long *)&rect->w;
  return;
}
