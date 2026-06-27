/* FUN_80038bbc @ 0x80038bbc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800390cc) */
/* WARNING: Removing unreachable block (ram,0x80039124) */

void FUN_80038bbc(void)

{
  char cVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  char unaff_s1;
  byte unaff_s2;
  byte bVar7;
  char unaff_s3;
  byte unaff_s5;
  byte bVar8;
  
  DAT_800e898c = '\0';
  uVar3 = FUN_80095d30(0);
  if (1 < uVar3) {
    DAT_800e8984 = FUN_80095df0(0,1,0);
    unaff_s1 = FUN_80095df0(0,2,0);
    if (DAT_800c3b40 != DAT_800e8984) {
      DAT_800e2a80 = '\0';
    }
    bVar7 = unaff_s2;
    bVar8 = unaff_s5;
    if ((DAT_800e8984 == '\x04') ||
       (bVar7 = DAT_800ce2ba, bVar8 = DAT_800ce2bb, DAT_800e8984 == '\a')) {
      unaff_s2 = bVar7;
      unaff_s5 = bVar8;
      if (DAT_800e2a80 == '\0') {
        FUN_800960e4(0,&DAT_800cbc20,2);
        if (uVar3 == 2) {
          DAT_800e2a80 = '\x01';
        }
        else if ((uVar3 == 6) && (iVar4 = FUN_80096064(0,&DAT_8009dba4), iVar4 != 0)) {
          DAT_800e2a80 = '\x02';
        }
      }
      goto LAB_80038d18;
    }
  }
  DAT_800ce2b7 = 0xff;
  DAT_800ce2b6 = 0xff;
  DAT_800ce2d8._3_1_ = 0xff;
  DAT_800ce2d8._2_1_ = 0xff;
  DAT_800e2a80 = '\0';
  DAT_800e8984 = '\0';
LAB_80038d18:
  DAT_800c3b40 = DAT_800e8984;
  if (unaff_s1 == '\0') {
    if (DAT_800d44a1 == '\0') {
      DAT_800cbc21 = 0;
      DAT_800cbc20 = 0;
      if ((((DAT_800e873c & 0x80) == 0) || ((DAT_800cfbd8 & 0x10) != 0)) &&
         ((DAT_800cfb74 & 0x2000) == 0)) {
        cVar1 = FUN_800396fc(&DAT_800eaad8);
        cVar2 = FUN_800396fc(&DAT_800eac28);
        if ((cVar1 != '\0') || (cVar2 != '\0')) {
          DAT_800cbc20 = 0x40;
          DAT_800cbc21 = 0xff;
        }
      }
    }
  }
  else if (DAT_800d44a1 == '\0') {
    DAT_800cbc21 = 0;
    DAT_800cbc20 = 0;
    if ((((DAT_800e873c & 0x80) == 0) || ((DAT_800cfbd8 & 0x10) != 0)) &&
       ((DAT_800cfb74 & 0x2000) == 0)) {
      DAT_800cbc20 = FUN_800396fc(&DAT_800eaad8);
      DAT_800cbc21 = FUN_800396fc(&DAT_800eac28);
    }
  }
  DAT_800ce314 = DAT_800ce30c;
  uVar3 = DAT_800ce2fc;
  if ((DAT_800ce5e1 & 0x80) != 0) {
    DAT_800cbc21 = 0;
    DAT_800cbc20 = 0;
  }
  DAT_800ce300 = DAT_800ce2fc;
  uVar6 = DAT_800cfb74;
  if ((DAT_800cfb74 & 0x2000) == 0) {
    DAT_800ce2fc = ~((uint)DAT_800ce2d8._2_1_ * 0x1000000 + (uint)DAT_800ce2d8._3_1_ * 0x10000 +
                     (uint)DAT_800ce2b6 * 0x100 | (uint)DAT_800ce2b7);
  }
  else {
    uVar5 = ~((uint)DAT_800ce2d8._2_1_ * 0x1000000 + (uint)DAT_800ce2d8._3_1_ * 0x10000 +
              (uint)DAT_800ce2b6 * 0x100 | (uint)DAT_800ce2b7);
    if ((((DAT_800ce2fc ^ uVar5) & uVar5 & 0xc00) == 0) && ((uint)DAT_800d4232 < (uint)DAT_800df3c4)
       ) {
      DAT_800ce2fc = uVar5;
      if ((DAT_800cfb74 & 0x40000000) != 0) {
        DAT_800ce2fc = (uint)*(ushort *)(&DAT_800df3ce + (uint)DAT_800d4232 * 2);
        DAT_800d4232 = DAT_800d4232 + 1;
      }
    }
    else {
      DAT_800ce2fc = uVar5;
      if ((DAT_800d7654 == '\0') &&
         (uVar6 = DAT_800cfb74 | 0x1000, (DAT_800cfb74 & 0x40000000) != 0)) {
        if (DAT_800d4232 < DAT_800df3c4) {
          DAT_800dfd5b = 1;
        }
        DAT_800ce2fc = 0;
        uVar6 = DAT_800cfb74;
        DAT_800d4232 = DAT_800df3c4 + 1;
      }
    }
  }
  DAT_800cfb74 = uVar6;
  if ((DAT_800cfb74 & 0x2000) == 0) {
    if ((DAT_800ce2fc & 0xa000) != 0) {
      DAT_800e898c = '\x01';
    }
    if (DAT_800e8984 == '\a') {
      DAT_800d785c = 0;
      unaff_s3 = '\0';
      if (unaff_s5 < 0x30) {
        DAT_800ce2fc = DAT_800ce2fc | 0x1000;
      }
      if (0xd0 < unaff_s5) {
        DAT_800ce2fc = DAT_800ce2fc | 0x4000;
      }
      if (unaff_s2 < 0x50) {
        if (unaff_s2 == 0) {
          DAT_800d785c = 10;
        }
        else {
          unaff_s2 = 0x50 - unaff_s2;
          DAT_800d785c = (undefined1)((int)(uint)unaff_s2 >> 3);
        }
        unaff_s3 = '\x01';
        DAT_800ce2fc = DAT_800ce2fc | 0x8000;
      }
      if (0xb0 < unaff_s2) {
        DAT_800d785c = 10;
        if (unaff_s2 != 0xff) {
          unaff_s2 = unaff_s2 + 0x50;
          DAT_800d785c = (undefined1)((int)(uint)unaff_s2 >> 3);
        }
        unaff_s3 = '\x01';
        DAT_800ce2fc = DAT_800ce2fc | 0x2000;
      }
      if ((DAT_800ce2fc & 0x400) != 0) {
        DAT_800ce2fc = DAT_800ce2fc | 0x800;
      }
    }
  }
  else {
    DAT_800e898c = '\x01';
  }
  DAT_800ce30c = 0;
  uVar6 = 0xf;
  do {
    uVar5 = uVar6 & 0xff;
    if ((DAT_800ce2fc & *(ushort *)(&UNK_800a26a0 + uVar5 * 2 + (uint)DAT_800d46b2 * 0x20)) != 0) {
      DAT_800ce30c = DAT_800ce30c | 1 << (uVar6 & 0x1f);
    }
    uVar6 = uVar6 - 1;
  } while (uVar5 != 0);
  if ((DAT_800cfbdc & 0x1000000) == 0) {
    if ((DAT_800a2700 & 0x1000000) != 0) {
      DAT_800a2700 = 0;
      DAT_800ce314 = DAT_800ce30c;
    }
  }
  else {
    DAT_800a2700 = DAT_800cfbdc;
    DAT_800ce30c = DAT_800ce30c & 0x3c00;
  }
  if (((DAT_800cfb74 & 0x2000) == 0) && (DAT_800e8984 == '\a')) {
    if (unaff_s3 == '\0') {
      if ((DAT_800ce30c & 10) != 0) {
        DAT_800d785c = 10;
      }
    }
    else {
      if (unaff_s2 != 0xff) {
        DAT_800ce2fc = DAT_800ce2fc & 0xffffdfff;
        DAT_800ce300 = uVar3 & 0xffffdfff;
      }
      if (unaff_s2 != 0) {
        DAT_800ce2fc = DAT_800ce2fc & 0xffff7fff;
        DAT_800ce300 = DAT_800ce300 & 0xffff7fff;
      }
    }
  }
  if (DAT_800e898c != '\0') {
    DAT_800d785c = 10;
  }
  DAT_800ce300 = (DAT_800ce300 ^ DAT_800ce2fc) & DAT_800ce2fc;
  DAT_800ce310 = (DAT_800ce314 ^ DAT_800ce30c) & DAT_800ce30c;
  DAT_800ce308 = (short)(DAT_800ce2fc >> 0x10);
  DAT_800ce306 = (short)DAT_800ce300;
  DAT_800ce304 = (short)DAT_800ce2fc;
  DAT_800ce30a = (short)(DAT_800ce300 >> 0x10);
  cVar1 = (char)DAT_800dfc18;
  if (((DAT_800ce300 & DAT_800dfc14) == 0) && (cVar1 = DAT_800dfc18._1_1_, DAT_800a2704 != '\0')) {
    if ((DAT_800ce2fc & DAT_800dfc14) != 0) {
      DAT_800a2704 = DAT_800a2704 + -1;
    }
    DAT_800cfb74 = DAT_800cfb74 & 0x7fffffff;
  }
  else {
    DAT_800cfb74 = DAT_800cfb74 | 0x80000000;
    DAT_800a2704 = cVar1;
  }
  return;
}


