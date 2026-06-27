int StoreImage(RECT *rect,u_long *p)

{
  int iVar1;
  
  checkRECT("StoreImage",rect);
  iVar1 = _addque2(_drs,rect,8,p);
  return iVar1;
}
