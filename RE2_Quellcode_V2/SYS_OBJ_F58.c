/* SYS_OBJ_F58 @ 0x80090e38  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_F58(undefined4 param_1,uint param_2,uint param_3)

{
  short sVar1;
  bool bVar2;
  char cVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  short *unaff_s0;
  int unaff_s1;
  uint unaff_s2;
  uint unaff_s3;
  
  uVar5 = param_2 + 0x50;
  if (((int)(param_2 + 0x50) <= (int)param_3) && (uVar5 = 0xcda, (int)param_3 < 0xcdb)) {
    uVar5 = param_3;
  }
  if (unaff_s1 < 0x10) {
    uVar4 = 0x10;
  }
  else {
    if ((char)unaff_s0[9] == '\0') {
      if (unaff_s1 < 0x101) {
        SYS_OBJ_FD8();
        return;
      }
    }
    else if (unaff_s1 < 0x137) {
      SYS_OBJ_FD8();
      return;
    }
    uVar4 = 0x100;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_FD8(0x136);
      return;
    }
  }
  uVar6 = uVar4 + 2;
  if ((int)(uVar4 + 2) <= (int)unaff_s2) {
    if ((char)unaff_s0[9] == '\0') {
      uVar6 = unaff_s2;
      if ((int)unaff_s2 < 0x103) goto code_r0x80090f14;
    }
    else if ((int)unaff_s2 < 0x139) {
      SYS_OBJ_1034();
      return;
    }
    uVar6 = 0x102;
    if ((char)unaff_s0[9] != '\0') {
      SYS_OBJ_1034(uVar4,0x138);
      return;
    }
  }
code_r0x80090f14:
  (**(code **)(PTR_PTR_800b26f8 + 0x10))((uVar5 & 0xfff) << 0xc | param_2 & 0xfff | 0x6000000);
  (**(code **)(PTR_PTR_800b26f8 + 0x10))((uVar6 & 0x3ff) << 10 | uVar4 | 0x7000000);
  if ((((DAT_800b277c != *(int *)(unaff_s0 + 8)) || (DAT_800b276c != *unaff_s0)) ||
      (DAT_800b276e != unaff_s0[1])) ||
     ((DAT_800b2770 != unaff_s0[2] || (DAT_800b2772 != unaff_s0[3])))) {
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


