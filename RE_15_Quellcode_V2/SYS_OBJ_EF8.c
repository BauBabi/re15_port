void SYS_OBJ_EF8(undefined4 param_1,uint param_2,uint param_3)

{
  short sVar1;
  bool bVar2;
  undefined1 uVar3;
  uint in_v1;
  short *unaff_s0;
  uint unaff_s1;
  uint unaff_s3;
  
  _ctl((param_3 & 0xfff) << 0xc | param_2 & 0xfff | 0x6000000);
  _ctl((in_v1 & 0x3ff) << 10 | unaff_s1 & 0x3ff | 0x7000000);
  if ((((*(int *)(unaff_s0 + 8) != 0) || (*unaff_s0 != 0)) || (unaff_s0[1] != 0)) ||
     ((unaff_s0[2] != 0 || (unaff_s0[3] != 0)))) {
    uVar3 = FUN_800614d0();
    *(undefined1 *)(unaff_s0 + 9) = uVar3;
    if ((char)unaff_s0[9] == '\x01') {
      unaff_s3 = unaff_s3 | 8;
    }
    if (*(char *)((int)unaff_s0 + 0x11) != '\0') {
      unaff_s3 = unaff_s3 | 0x10;
    }
    if ((char)unaff_s0[8] != '\0') {
      unaff_s3 = unaff_s3 | 0x20;
    }
    if (DAT_8007e353 != '\0') {
      unaff_s3 = unaff_s3 | 0x80;
    }
    sVar1 = unaff_s0[2];
    if (0x118 < sVar1) {
      if (sVar1 < 0x161) {
        SYS_OBJ_1088();
        return;
      }
      if (sVar1 < 0x191) {
        SYS_OBJ_1088();
        return;
      }
      if (sVar1 < 0x231) {
        SYS_OBJ_1088();
        return;
      }
      unaff_s3 = unaff_s3 | 3;
    }
    bVar2 = unaff_s0[3] < 0x121;
    if ((char)unaff_s0[9] == '\0') {
      bVar2 = unaff_s0[3] < 0x101;
    }
    if (!bVar2) {
      unaff_s3 = unaff_s3 | 0x24;
    }
    _ctl(unaff_s3);
  }
  memcpy("",(uchar *)unaff_s0,0x14);
  return;
}
