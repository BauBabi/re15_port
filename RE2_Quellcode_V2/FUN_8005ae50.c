/* FUN_8005ae50 @ 0x8005ae50  (Ghidra headless, raw MIPS overlay) */

uint FUN_8005ae50(void)

{
  bool bVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  ulong uVar7;
  int iVar8;
  int iVar9;
  
  uVar3 = DAT_800cfb74 & 0x2000;
  if (uVar3 == 0) {
    DAT_800e2a84 = 1;
    iVar8 = 0;
    iVar9 = 0;
    do {
      while( true ) {
        FUN_80031f94(1);
        SpuSetTransferMode(0);
        iVar5 = (int)DAT_800d75ca;
        if (-1 < iVar5) {
          if (DAT_800d75c8 != '\0') {
            sVar2 = SsIsEos((short)DAT_800d75ca,0);
            if (sVar2 != 0) {
              FUN_8007eaf8((int)DAT_800d75ca);
            }
            DAT_800d75c8 = '\0';
            iVar5 = (int)DAT_800d75ca;
          }
          SsSeqClose((short)iVar5);
          DAT_800d75ca = -1;
        }
        iVar5 = (int)DAT_800d75d2;
        if (-1 < iVar5) {
          if (DAT_800d75d0 != '\0') {
            sVar2 = SsIsEos((short)DAT_800d75d2,0);
            if (sVar2 != 0) {
              FUN_8007eaf8((int)DAT_800d75d2);
            }
            DAT_800d75d0 = '\0';
            iVar5 = (int)DAT_800d75d2;
          }
          SsSeqClose((short)iVar5);
          DAT_800d75d2 = -1;
        }
        if (DAT_800d4c4e != -1) {
          SsVabClose((short)DAT_800d4c4e);
          DAT_800d4c4e = -1;
        }
        if (*(byte *)(DAT_800dfd64 + 1) == 0xff) {
          return 0xffffffff;
        }
        uVar3 = DAT_800ce32c + 0x10U & 0xfffffff0;
        iVar5 = FUN_80012fb8(*(undefined2 *)
                              (&DAT_800a81e8 + (*(byte *)(DAT_800dfd64 + 1) & 0x3f) * 2),uVar3,1,
                             "              SUB BGM");
        iVar6 = uVar3 + iVar5;
        if (iVar5 == -1) {
          return 0xffffffff;
        }
        iVar4 = *(int *)(iVar6 + -8);
        iVar5 = *(int *)(iVar6 + -0xc);
        iVar6 = *(int *)(iVar6 + -0x10);
        DAT_800d75b8 = &DAT_801ece00 + iVar5;
        FUN_80076a00(&DAT_801ece00);
        if (0x38800 < *(int *)(DAT_800d75b8 + 0xc) + -0xa20 +
                      (uint)*(ushort *)(DAT_800d75b8 + 0x12) * -0x200 + DAT_800eade4) {
          iVar8 = 0x95;
          do {
            bVar1 = iVar8 != 0;
            iVar8 = iVar8 + -1;
          } while (bVar1);
          return 0xffffffff;
        }
        uVar7 = DAT_800eade4 + 0x42fc0;
        do {
          FUN_80031f94(1);
          sVar2 = SsVabTransCompleted(0);
        } while (sVar2 == 0);
        sVar2 = SsVabOpenHeadSticky(&DAT_801ece00 + iVar5,6,uVar7);
        DAT_800d4c4e = (char)sVar2;
        iVar8 = iVar8 + 1;
        if (DAT_800d4c4e == -1) break;
        iVar8 = 0;
        sVar2 = SsVabTransBody((uchar *)(uVar3 + iVar6),(short)DAT_800d4c4e);
        iVar9 = iVar9 + 1;
        if (sVar2 != -1) {
          DAT_800a7fac = 0;
          while (sVar2 = SsVabTransCompleted(0), sVar2 == 0) {
            FUN_8005cf70();
            FUN_80031f94(1);
          }
          sVar2 = SsSeqOpen((ulong *)&DAT_801ece00,(short)DAT_800d4c4e);
          DAT_800d75ca = (char)sVar2;
          if (-1 < DAT_800d75ca) {
            SsSeqPlay((short)DAT_800d75ca,'\0',1);
            iVar8 = (int)((((uint)DAT_800d75cc * (uint)DAT_800eae3c) / 100) * 0x10000) >> 0x10;
            FUN_8007f018((int)DAT_800d75ca,iVar8,iVar8);
          }
          sVar2 = SsSeqOpen((ulong *)(&DAT_801ece00 + iVar4),(short)DAT_800d4c4e);
          DAT_800d75d2 = (char)sVar2;
          if (-1 < DAT_800d75d2) {
            SsSeqPlay((short)DAT_800d75d2,'\0',1);
            iVar8 = (int)((((uint)DAT_800d75d4 * (uint)DAT_800eae3c) / 100) * 0x10000) >> 0x10;
            FUN_8007f018((int)DAT_800d75d2,iVar8,iVar8);
          }
          DAT_800d75c8 = 0;
          DAT_800d75d0 = 0;
          DAT_800e2a84 = 0;
          return 0;
        }
        FUN_800594c8((int)DAT_800d4c4e,uVar7);
        if (iVar9 == 8) goto LAB_8005b154;
      }
      FUN_800594c8(6,0);
    } while (iVar8 != 8);
LAB_8005b154:
    uVar3 = FUN_80031f94(1);
  }
  return uVar3;
}


