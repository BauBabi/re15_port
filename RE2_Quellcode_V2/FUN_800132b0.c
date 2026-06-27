/* FUN_800132b0 @ 0x800132b0  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800132b0(int param_1,int param_2,int param_3,int param_4)

{
  bool bVar1;
  byte bVar2;
  short sVar3;
  short sVar4;
  uint uVar5;
  int iVar6;
  undefined1 *puVar7;
  int iVar8;
  
  do {
    iVar8 = 0;
    puVar7 = &DAT_800d5300;
    if (param_2 == 0) {
      iVar6 = param_1 * 8;
      DAT_800d531e = (&DAT_800988ab)[iVar6];
      DAT_800d5308 = *(int *)(&DAT_800988a4 + iVar6);
      DAT_800d5314 = (uint)(byte)(&DAT_800988aa)[iVar6] * 0x10000 +
                     (uint)*(ushort *)(&DAT_800988a8 + iVar6);
    }
    do {
      (&DAT_800d5b5c)[iVar8] = 0;
      *(undefined2 *)(puVar7 + 0x868) = 0;
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 2;
    } while (iVar8 < 0xc);
    DAT_800d7668 = 0;
    DAT_800d5300 = 1;
    DAT_800d5303 = 2;
    DAT_800d531c = (undefined2)param_1;
    DAT_800d531a = 0;
    DAT_800d5310 = 0;
    DAT_800d5335 = 0;
    DAT_800d5320 = 0;
    DAT_800d531f = '\0';
    DAT_800d5b59 = 0;
    DAT_800d5b58 = 0;
    DAT_800d5b5a = '\0';
    DAT_800d5336 = 0;
    DAT_800d5318 = (short)(DAT_800d5308 + 0x7ffU >> 0xb);
    DAT_800d5328 = DAT_800d5314;
    DAT_800d530c = DAT_800d5308;
    DAT_800d5b54 = param_3;
    DsFlush();
    DsIntToPos(DAT_800d5328,(DslLOC *)&DAT_800d5304);
    DsSyncCallback((DslCB)&LAB_80013714);
    do {
      iVar8 = DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
    } while (iVar8 == 0);
    do {
      iVar8 = DsCommand('\x0e',&DAT_8009a42a,(DslCB)0x0,-1);
    } while (iVar8 == 0);
    do {
      iVar8 = DsCommand('\x15',"",(DslCB)0x0,-1);
    } while (iVar8 == 0);
    DsReadyCallback((DslCB)&LAB_8001376c);
    do {
      iVar8 = DsCommand('\x06',"",(DslCB)0x0,-1);
    } while (iVar8 == 0);
    while( true ) {
      iVar8 = DsSystemStatus();
      if (iVar8 == 1) {
        bVar2 = DsStatus();
        if ((bVar2 & 0x20) == 0) {
          DAT_800d5300 = 6;
        }
        else if ((DAT_800d530c == 0) && (DAT_800d5318 == 0)) {
          if (DAT_800d531f == DAT_800d531e) {
            DAT_800d5300 = 0;
          }
          else {
            DAT_800d5300 = 4;
          }
          DAT_800d5335 = 0;
          do {
            iVar8 = DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
          } while (iVar8 == 0);
        }
      }
      while ((&DAT_800d5b5c)[DAT_800d5b59] != '\0') {
        if (DAT_800d5b5a == '\0') {
          DAT_800d5b5a = '\x01';
        }
        else {
          DAT_800d7668 = 0;
          while (sVar3 = SsVabTransCompleted(0), sVar3 == 0) {
            uVar5 = DAT_800d7668 + 1;
            bVar1 = 3000 < DAT_800d7668;
            DAT_800d7668 = uVar5;
            if (bVar1) {
              FUN_800957a4();
              DeliverEvent(0xf0000009,0x20);
              FUN_800957b4();
            }
          }
        }
        sVar3 = SsVabTransBodyPartly
                          ((uchar *)(DAT_800d5b54 + (uint)DAT_800d5b59 * 0x800),
                           (uint)(ushort)(&DAT_800d5b68)[DAT_800d5b59],(short)param_4);
        if (sVar3 == param_4) {
          DAT_800d7668 = 0;
          while (sVar4 = SsVabTransCompleted(0), sVar4 == 0) {
            uVar5 = DAT_800d7668 + 1;
            bVar1 = 3000 < DAT_800d7668;
            DAT_800d7668 = uVar5;
            if (bVar1) {
              FUN_800957a4();
              DeliverEvent(0xf0000009,0x20);
              FUN_800957b4();
            }
          }
        }
        if (sVar3 == -1) {
          DsSyncCallback((DslCB)0x0);
          DsReadyCallback((DslCB)0x0);
          do {
            sVar3 = SsVabTransCompleted(0);
          } while (sVar3 == 0);
          FUN_80031f94(0x1e);
          DAT_800d5300 = 0;
          return 0xffffffff;
        }
        (&DAT_800d5b5c)[DAT_800d5b59] = 0;
        DAT_800d5b59 = DAT_800d5b59 + 1;
        if (DAT_800d5b59 == 0xc) {
          DAT_800d5b59 = 0;
        }
      }
      FUN_80031f94(1);
      if (1 < DAT_800d5300) break;
      if (DAT_800d5300 == 0) {
        return DAT_800d5310;
      }
    }
  } while( true );
}


