void SYS_OBJ_1088(void)

{
  bool bVar1;
  uchar *unaff_s0;
  uint unaff_s3;
  
  bVar1 = *(short *)(unaff_s0 + 6) < 0x121;
  if (unaff_s0[0x12] == '\0') {
    bVar1 = *(short *)(unaff_s0 + 6) < 0x101;
  }
  if (!bVar1) {
    unaff_s3 = unaff_s3 | 0x24;
  }
  _ctl(unaff_s3);
  memcpy("",unaff_s0,0x14);
  return;
}
