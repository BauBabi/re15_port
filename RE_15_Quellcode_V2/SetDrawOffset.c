void SetDrawOffset(DR_OFFSET *p,u_short *ofs)

{
  u_long uVar1;
  
  *(undefined1 *)((int)&p->tag + 3) = 2;
  uVar1 = get_ofs((int)(short)*ofs,(int)(short)ofs[1]);
  p->code[0] = uVar1;
  p->code[1] = 0;
  return;
}
