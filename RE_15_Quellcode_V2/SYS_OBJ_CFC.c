void SYS_OBJ_CFC(void)

{
  short sVar1;
  bool bVar2;
  undefined1 uVar3;
  uint in_v0;
  uint in_v1;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  short *unaff_s0;
  int iVar9;
  uint uVar10;
  uint unaff_s3;
  
  _ctl(in_v0 | in_v1);
  if ((((unaff_s0[4] == 0) && (unaff_s0[5] == 0)) && (unaff_s0[6] == 0)) && (unaff_s0[7] == 0))
  goto SYS_OBJ_F54;
  uVar3 = FUN_800614d0();
  *(undefined1 *)(unaff_s0 + 9) = uVar3;
  uVar7 = unaff_s0[4] * 10 + 0x260;
  iVar5 = (int)unaff_s0[5];
  iVar9 = iVar5 + 0x13;
  if ((char)unaff_s0[9] == '\0') {
    iVar9 = iVar5 + 0x10;
  }
  if (unaff_s0[6] != 0) {
    SYS_OBJ_DE8(iVar5,uVar7,uVar7 + unaff_s0[6] * 10);
    return;
  }
  uVar8 = unaff_s0[4] * 10 + 0xc60;
  uVar10 = iVar9 + unaff_s0[7];
  if (unaff_s0[7] == 0) {
    uVar10 = iVar9 + 0xf0;
  }
  uVar6 = 500;
  if ((499 < (int)uVar7) && (uVar6 = uVar7, 0xcda < (int)uVar7)) {
    uVar6 = 0xcda;
  }
  uVar7 = uVar6 + 0x50;
  if (((int)(uVar6 + 0x50) <= (int)uVar8) && (uVar7 = uVar8, 0xcda < (int)uVar8)) {
    uVar7 = 0xcda;
  }
  if (iVar9 < 0x10) {
    uVar8 = 0x10;
  }
  else {
    if ((char)unaff_s0[9] == '\0') {
      if (iVar9 < 0x101) {
        SYS_OBJ_E9C(iVar9);
        return;
      }
    }
    else if (iVar9 < 0x137) {
      SYS_OBJ_E9C(iVar9);
      return;
    }
    uVar8 = 0x100;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_E9C(0x136);
      return;
    }
  }
  uVar4 = uVar8 + 2;
  if ((int)(uVar8 + 2) <= (int)uVar10) {
    if ((char)unaff_s0[9] == '\0') {
      uVar4 = uVar10;
      if ((int)uVar10 < 0x103) goto code_r0x800694bc;
    }
    else if ((int)uVar10 < 0x139) {
      SYS_OBJ_EF8();
      return;
    }
    uVar4 = 0x102;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_EF8();
      return;
    }
  }
code_r0x800694bc:
  _ctl((uVar7 & 0xfff) << 0xc | uVar6 & 0xfff | 0x6000000);
  _ctl((uVar4 & 0x3ff) << 10 | uVar8 | 0x7000000);
SYS_OBJ_F54:
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
