/* SetDrawLoad @ 0x8008fc7c  (Ghidra headless, raw MIPS overlay) */

void SetDrawLoad(DR_LOAD *p,RECT *rect)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = ((int)rect->w * (int)rect->h + 1) / 2;
  iVar2 = uVar1 + 4;
  if (0xc < uVar1) {
    iVar2 = 0;
  }
  *(char *)((int)&p->tag + 3) = (char)iVar2;
  p->code[0] = 0xa0000000;
  p->code[1] = *(u_long *)rect;
  p->code[2] = *(u_long *)&rect->w;
  p->code[iVar2 + -1] = 0x1000000;
  return;
}


