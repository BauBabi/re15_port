/* SYS_OBJ_E34 @ 0x80090d14  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_E34(void)

{
  short sVar1;
  bool bVar2;
  char cVar3;
  uint in_v0;
  uint in_v1;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  short *unaff_s0;
  int iVar11;
  uint uVar12;
  uint unaff_s3;
  
  (**(code **)(PTR_PTR_800b26f8 + 0x10))(in_v0 | in_v1);
  if ((((DAT_800b2774 == unaff_s0[4]) && (DAT_800b2776 == unaff_s0[5])) &&
      (DAT_800b2778 == unaff_s0[6])) && (DAT_800b277a == unaff_s0[7])) goto SYS_OBJ_1090;
  cVar3 = FUN_80086b7c();
  *(char *)(unaff_s0 + 9) = cVar3;
  iVar5 = (int)unaff_s0[5];
  iVar4 = unaff_s0[4] * 10 + 0x260;
  if (cVar3 != '\0') {
    SYS_OBJ_F00();
    return;
  }
  iVar11 = iVar5 + 0x10;
  iVar6 = (int)unaff_s0[6];
  if (iVar6 != 0) {
    SYS_OBJ_F24(iVar5,iVar6,iVar4 + iVar6 * 10);
    return;
  }
  uVar10 = unaff_s0[4] * 10 + 0xc60;
  uVar12 = iVar11 + unaff_s0[7];
  if (unaff_s0[7] == 0) {
    uVar12 = iVar5 + 0x100;
  }
  if (iVar4 < 500) {
    uVar7 = 500;
  }
  else {
    uVar7 = 0xcda;
    if (iVar4 < 0xcdb) {
      SYS_OBJ_F58(iVar5,iVar4);
      return;
    }
  }
  uVar8 = uVar7 + 0x50;
  if (((int)(uVar7 + 0x50) <= (int)uVar10) && (uVar8 = 0xcda, (int)uVar10 < 0xcdb)) {
    uVar8 = uVar10;
  }
  if (iVar11 < 0x10) {
    uVar10 = 0x10;
  }
  else {
    if ((char)unaff_s0[9] == '\0') {
      if (iVar11 < 0x101) {
        SYS_OBJ_FD8(iVar11);
        return;
      }
    }
    else if (iVar11 < 0x137) {
      SYS_OBJ_FD8(iVar11);
      return;
    }
    uVar10 = 0x100;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_FD8(0x136);
      return;
    }
  }
  uVar9 = uVar10 + 2;
  if ((int)(uVar10 + 2) <= (int)uVar12) {
    if ((char)unaff_s0[9] == '\0') {
      uVar9 = uVar12;
      if ((int)uVar12 < 0x103) goto code_r0x80090f14;
    }
    else if ((int)uVar12 < 0x139) {
      SYS_OBJ_1034(uVar10,uVar12);
      return;
    }
    uVar9 = 0x102;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_1034(uVar10,0x138);
      return;
    }
  }
code_r0x80090f14:
  (**(code **)(PTR_PTR_800b26f8 + 0x10))((uVar8 & 0xfff) << 0xc | uVar7 | 0x6000000);
  (**(code **)(PTR_PTR_800b26f8 + 0x10))((uVar9 & 0x3ff) << 10 | uVar10 | 0x7000000);
SYS_OBJ_1090:
  if (((DAT_800b277c != *(int *)(unaff_s0 + 8)) || (DAT_800b276c != *unaff_s0)) ||
     ((DAT_800b276e != unaff_s0[1] ||
      ((DAT_800b2770 != unaff_s0[2] || (DAT_800b2772 != unaff_s0[3])))))) {
    cVar3 = FUN_80086b7c();
    *(char *)(unaff_s0 + 9) = cVar3;
    if (cVar3 == '\x01') {
      unaff_s3 = unaff_s3 | 8;
    }
    if (*(char *)((int)unaff_s0 + 0x11) != '\0') {
      unaff_s3 = unaff_s3 | 0x10;
    }
    if ((char)unaff_s0[8] != '\0') {
      unaff_s3 = unaff_s3 | 0x20;
    }
    if (DAT_800b2703 != '\0') {
      unaff_s3 = unaff_s3 | 0x80;
    }
    sVar1 = unaff_s0[2];
    if (0x118 < sVar1) {
      if (sVar1 < 0x161) {
        SYS_OBJ_11C0();
        return;
      }
      if (sVar1 < 0x191) {
        SYS_OBJ_11C0();
        return;
      }
      if (sVar1 < 0x231) {
        SYS_OBJ_11C0();
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
    (**(code **)(PTR_PTR_800b26f8 + 0x10))(unaff_s3);
  }
  memcpy("",(uchar *)unaff_s0,0x14);
  return;
}


