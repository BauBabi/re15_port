/* FUN_8005a97c @ 0x8005a97c  (Ghidra headless, raw MIPS overlay) */

uint FUN_8005a97c(void)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  uVar2 = DAT_800cfb74 & 0x2000;
  if (uVar2 == 0) {
    DAT_800e2a84 = 1;
    iVar6 = 0;
    iVar7 = 0;
    do {
      while( true ) {
        FUN_80031f94(1);
        SpuSetTransferMode(0);
        DAT_800eade4 = 0;
        if (-1 < DAT_800d75c2) {
          if (DAT_800d75c0 != '\0') {
            sVar1 = SsIsEos((short)DAT_800d75c2,0);
            if (sVar1 != 0) {
              FUN_8007eaf8((int)DAT_800d75c2);
            }
            DAT_800d75c0 = '\0';
          }
          SsSeqClose((short)DAT_800d75c2);
          DAT_800d75c2 = -1;
        }
        if (DAT_800d4c4d != -1) {
          SsVabClose((short)DAT_800d4c4d);
          DAT_800d4c4d = -1;
        }
        if (*DAT_800dfd64 == 0xff) {
          return 0xffffffff;
        }
        iVar4 = (int)DAT_800d75ca;
        if (-1 < iVar4) {
          if (DAT_800d75c8 != '\0') {
            sVar1 = SsIsEos((short)DAT_800d75ca,0);
            if (sVar1 != 0) {
              FUN_8007eaf8((int)DAT_800d75ca);
            }
            DAT_800d75c8 = '\0';
            iVar4 = (int)DAT_800d75ca;
          }
          SsSeqClose((short)iVar4);
          DAT_800d75ca = -1;
        }
        iVar4 = (int)DAT_800d75d2;
        if (-1 < iVar4) {
          if (DAT_800d75d0 != '\0') {
            sVar1 = SsIsEos((short)DAT_800d75d2,0);
            if (sVar1 != 0) {
              FUN_8007eaf8((int)DAT_800d75d2);
            }
            DAT_800d75d0 = '\0';
            iVar4 = (int)DAT_800d75d2;
          }
          SsSeqClose((short)iVar4);
          DAT_800d75d2 = -1;
        }
        if (DAT_800d4c4e != -1) {
          SsVabClose((short)DAT_800d4c4e);
          DAT_800d4c4e = -1;
        }
        uVar2 = DAT_800ce32c + 0x10U & 0xfffffff0;
        iVar4 = FUN_80012fb8(*(undefined2 *)(&DAT_800a8168 + (*DAT_800dfd64 & 0x3f) * 2),uVar2,1,
                             "              MAIN BGM");
        iVar5 = uVar2 + iVar4;
        if (iVar4 == -1) {
          return 0xffffffff;
        }
        iVar3 = *(int *)(iVar5 + -4);
        iVar4 = *(int *)(iVar5 + -8);
        iVar5 = *(int *)(iVar5 + -0xc);
        DAT_800d75b4 = &DAT_801f2e00 + iVar4;
        FUN_80076a00(&DAT_801f2e00);
        DAT_800eade4 = *(int *)(DAT_800d75b4 + 0xc) + -0xa20 +
                       (uint)*(ushort *)(DAT_800d75b4 + 0x12) * -0x200;
        if (0x38800 < DAT_800eade4) {
          DAT_800eade4 = 0x96;
          do {
            FUN_80031f94(1);
            DAT_800eade4 = DAT_800eade4 + -1;
          } while (DAT_800eade4 != 0);
          return 0xffffffff;
        }
        do {
          FUN_80031f94(1);
          sVar1 = SsVabTransCompleted(0);
        } while (sVar1 == 0);
        sVar1 = SsVabOpenHeadSticky(&DAT_801f2e00 + iVar4,5,0x42fc0);
        DAT_800d4c4d = (char)sVar1;
        iVar6 = iVar6 + 1;
        if (DAT_800d4c4d == -1) break;
        iVar6 = 0;
        sVar1 = SsVabTransBody((uchar *)(uVar2 + iVar5),(short)DAT_800d4c4d);
        iVar7 = iVar7 + 1;
        if (sVar1 != -1) {
          DAT_800a7fac = 0;
          while (sVar1 = SsVabTransCompleted(0), sVar1 == 0) {
            FUN_8005cf70();
            FUN_80031f94(1);
          }
          sVar1 = SsSeqOpen((ulong *)(&DAT_801f2e00 + iVar3),(short)DAT_800d4c4d);
          DAT_800d75c2 = (char)sVar1;
          if (-1 < DAT_800d75c2) {
            SsSeqPlay((short)DAT_800d75c2,'\0',1);
            iVar6 = (int)((((uint)DAT_800d75c4 * (uint)DAT_800eae3c) / 100) * 0x10000) >> 0x10;
            FUN_8007f018((int)DAT_800d75c2,iVar6,iVar6);
          }
          DAT_800d75c0 = 0;
          FUN_80031f94(1);
          DAT_800e2a84 = 0;
          return 0;
        }
        FUN_800594c8((int)DAT_800d4c4d,0x42fc0);
        if (iVar7 == 8) goto LAB_8005ad48;
      }
      FUN_800594c8(5,0);
    } while (iVar6 != 8);
LAB_8005ad48:
    uVar2 = FUN_80031f94(1);
  }
  return uVar2;
}


