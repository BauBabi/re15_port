/* SetDrawMode @ 0x800912ac  (Ghidra headless, raw MIPS overlay) */

void SetDrawMode(DR_MODE *p,int dfe,int dtd,int tpage,RECT *tw)

{
  u_long uVar1;
  
  *(undefined1 *)((int)&p->tag + 3) = 2;
  uVar1 = get_mode(dfe,dtd,tpage & 0xffff);
  p->code[0] = uVar1;
  uVar1 = get_tw(tw);
  p->code[1] = uVar1;
  return;
}


