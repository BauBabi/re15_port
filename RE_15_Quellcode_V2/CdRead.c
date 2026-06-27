int CdRead(int sectors,u_long *buf,int mode)

{
  int iVar1;
  
  DAT_80078804 = mode;
  if ((mode & 0x30U) == 0) {
    DAT_80078808 = 0x200;
    iVar1 = CDREAD_OBJ_468();
    return iVar1;
  }
  if ((mode & 0x30U) != 0x20) {
    iVar1 = CDREAD_OBJ_45C();
    return iVar1;
  }
  DAT_80078808 = 0x249;
  iVar1 = CDREAD_OBJ_468();
  return iVar1;
}
