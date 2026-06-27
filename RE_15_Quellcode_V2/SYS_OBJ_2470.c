uint SYS_OBJ_2470(void)

{
  undefined4 unaff_s2;
  undefined4 unaff_s3;
  
  *(undefined4 *)(&DAT_800b9cbc + DAT_8007e454 * 0x60) = unaff_s2;
  *(undefined4 *)(&DAT_800b9cb4 + DAT_8007e454 * 0x60) = unaff_s3;
  DAT_8007e454 = DAT_8007e454 + 1 & 0x3f;
  SetIntrMask(DAT_8007e45c);
  _exeque();
  return DAT_8007e454 - DAT_8007e458 & 0x3f;
}
