int CdReadSync(int mode,u_char *result)

{
  int iVar1;
  int iVar2;
  
  while( true ) {
    iVar1 = FUN_80061fc0(0xffffffff);
    iVar2 = -1;
    if ((iVar1 <= DAT_80078814 + 0x4b0) &&
       ((DAT_8007880c < 0 ||
        (iVar1 = FUN_80061fc0(0xffffffff), iVar2 = DAT_8007880c, DAT_80078810 + 0x3c < iVar1))))
    break;
    if ((mode != 0) || (iVar2 < 1)) {
      CdReady(1,result);
      return iVar2;
    }
  }
  cd_read_retry(1);
  iVar2 = CDREAD_OBJ_594();
  return iVar2;
}
