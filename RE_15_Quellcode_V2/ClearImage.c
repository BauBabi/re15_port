int ClearImage(RECT *rect,u_char r,u_char g,u_char b)

{
  int iVar1;
  
  checkRECT("ClearImage",rect);
  iVar1 = _addque2(_clr,rect,8,(uint)b << 0x10 | (uint)g << 8 | (uint)r);
  return iVar1;
}
