void DrawPrim(void *p)

{
  undefined1 uVar1;
  
  uVar1 = *(undefined1 *)((int)p + 3);
  _sync(0);
  _cwb((int)p + 4,uVar1);
  return;
}
