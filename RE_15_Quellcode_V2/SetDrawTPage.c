void SetDrawTPage(DR_TPAGE *p,int dfe,int dtd,int tpage)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  *(undefined1 *)((int)&p->tag + 3) = 1;
  iVar1 = GetGraphType();
  if ((iVar1 != 1) && (iVar1 = GetGraphType(), iVar1 != 2)) {
    uVar3 = 0xe1000000;
    if (dtd != 0) {
      uVar3 = 0xe1000200;
    }
    uVar2 = tpage & 0x9ff;
    if (dfe != 0) {
      uVar2 = uVar2 | 0x400;
    }
    p->code[0] = uVar3 | uVar2;
    return;
  }
  PRIM_OBJ_5C0();
  return;
}
