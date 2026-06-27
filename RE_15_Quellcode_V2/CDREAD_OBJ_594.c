int CDREAD_OBJ_594(void)

{
  int iVar1;
  int unaff_s0;
  int *unaff_s1;
  int unaff_s2;
  u_char *unaff_s3;
  
  while( true ) {
    do {
      if ((unaff_s2 != 0) || (unaff_s0 < 1)) {
        CdReady(1,unaff_s3);
        return unaff_s0;
      }
      iVar1 = FUN_80061fc0(0xffffffff);
      unaff_s0 = -1;
    } while (*unaff_s1 + 0x4b0 < iVar1);
    if ((unaff_s1[-2] < 0) || (iVar1 = FUN_80061fc0(0xffffffff), unaff_s1[-1] + 0x3c < iVar1))
    break;
    unaff_s0 = unaff_s1[-2];
  }
  cd_read_retry(1);
  iVar1 = CDREAD_OBJ_594();
  return iVar1;
}
