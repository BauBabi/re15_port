void SYS_OBJ_DE8(undefined4 param_1,uint param_2,uint param_3)

{
  short sVar1;
  bool bVar2;
  undefined1 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  short *unaff_s0;
  int unaff_s1;
  uint uVar8;
  uint unaff_s3;
  
  uVar8 = unaff_s1 + unaff_s0[7];
  if (unaff_s0[7] == 0) {
    uVar8 = unaff_s1 + 0xf0;
  }
  uVar6 = 500;
  if ((499 < (int)param_2) && (uVar6 = param_2, 0xcda < (int)param_2)) {
    uVar6 = 0xcda;
  }
  uVar4 = uVar6 + 0x50;
  if (((int)(uVar6 + 0x50) <= (int)param_3) && (uVar4 = param_3, 0xcda < (int)param_3)) {
    uVar4 = 0xcda;
  }
  if (unaff_s1 < 0x10) {
    uVar7 = 0x10;
  }
  else {
    if ((char)unaff_s0[9] == '\0') {
      if (unaff_s1 < 0x101) {
        SYS_OBJ_E9C();
        return;
      }
    }
    else if (unaff_s1 < 0x137) {
      SYS_OBJ_E9C();
      return;
    }
    uVar7 = 0x100;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_E9C(0x136);
      return;
    }
  }
  uVar5 = uVar7 + 2;
  if ((int)(uVar7 + 2) <= (int)uVar8) {
    if ((char)unaff_s0[9] == '\0') {
      uVar5 = uVar8;
      if ((int)uVar8 < 0x103) goto code_r0x800694bc;
    }
    else if ((int)uVar8 < 0x139) {
      SYS_OBJ_EF8();
      return;
    }
    uVar5 = 0x102;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_EF8();
      return;
    }
  }
code_r0x800694bc:
  _ctl((uVar4 & 0xfff) << 0xc | uVar6 & 0xfff | 0x6000000);
  _ctl((uVar5 & 0x3ff) << 10 | uVar7 | 0x7000000);
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
