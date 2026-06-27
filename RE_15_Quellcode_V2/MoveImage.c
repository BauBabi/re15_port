int MoveImage(RECT *rect,int x,int y)

{
  int iVar1;
  
  checkRECT("MoveImage",rect);
  iVar1 = -1;
  if (rect->w != 0) {
    if (rect->h == 0) {
      iVar1 = SYS_OBJ_834();
      return iVar1;
    }
    DAT_8007e404 = y << 0x10 | x & 0xffffU;
    DAT_8007e400._0_2_ = rect->x;
    DAT_8007e400._2_2_ = rect->y;
    DAT_8007e408._0_2_ = rect->w;
    DAT_8007e408._2_2_ = rect->h;
    iVar1 = _addque2(_cwc,&DAT_8007e3f8,0x14,0);
  }
  return iVar1;
}
