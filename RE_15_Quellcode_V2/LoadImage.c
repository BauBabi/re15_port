int LoadImage(RECT *rect,u_long *p)

{
  int iVar1;
  
  checkRECT("LoadImage",rect);
  iVar1 = _addque2(_dws,rect,8,p);
  return iVar1;
}
