void SetPriority(DR_PRIO *p,int pbc,int pbw)

{
  uint uVar1;
  
  *(undefined1 *)((int)&p->tag + 3) = 2;
  uVar1 = 0xe6000000;
  if (pbc != 0) {
    uVar1 = 0xe6000002;
  }
  p->code[0] = uVar1 | pbw != 0;
  p->code[1] = 0;
  return;
}
