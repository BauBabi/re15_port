/* FUN_80012fb8 @ 0x80012fb8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80012fb8(int param_1,undefined4 param_2,uint param_3)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
LAB_80012ff4:
  iVar2 = param_1 * 8;
  if (param_3 < 2) {
    DAT_800d531e = (&DAT_800988ab)[iVar2];
    DAT_800d5308 = *(int *)(&DAT_800988a4 + iVar2);
    DAT_800d5314 = (uint)(byte)(&DAT_800988aa)[iVar2] * 0x10000 +
                   (uint)*(ushort *)(&DAT_800988a8 + iVar2);
    uVar3 = param_3;
  }
  else {
    uVar3 = param_3 - 2;
  }
  DAT_8009a410 = 0;
  DAT_800d5300 = 1;
  DAT_800d5303 = 1;
  DAT_800d531c = (undefined2)param_1;
  DAT_800d531a = 0;
  DAT_800d5310 = 0;
  DAT_800d5335 = 0;
  DAT_800d5320 = 0;
  DAT_800d531f = '\0';
  DAT_800d5336 = 0;
  DAT_800d5318 = (short)(DAT_800d5308 + 0x7ffU >> 0xb);
  DAT_800d5328 = DAT_800d5314;
  DAT_800d530c = DAT_800d5308;
  DAT_800d5330 = param_2;
  DsFlush();
  DsIntToPos(DAT_800d5328,(DslLOC *)&DAT_800d5304);
  DsSyncCallback((DslCB)&LAB_80013714);
  do {
    iVar2 = DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsCommand('\x0e',&DAT_8009a429,(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsCommand('\x15',"",(DslCB)0x0,-1);
  } while (iVar2 == 0);
  DsReadyCallback((DslCB)&LAB_8001376c);
  do {
    iVar2 = DsCommand('\x06',"",(DslCB)0x0,-1);
  } while (iVar2 == 0);
  do {
    iVar2 = DsSystemStatus();
    if (iVar2 == 1) {
      bVar1 = DsStatus();
      if ((bVar1 & 0x20) == 0) {
LAB_8001323c:
        DAT_800d5300 = 6;
      }
      else if ((DAT_800d530c == 0) && (DAT_800d5318 == 0)) {
        if ((DAT_800d531f == DAT_800d531e) || (DAT_800d5302 == '\x01')) {
          DAT_800d5300 = 0;
        }
        else {
          DAT_800d5300 = 4;
        }
        DAT_800d5335 = 0;
        do {
          iVar2 = DsCommand('\t',(u_char *)0x0,(DslCB)0x0,-1);
        } while (iVar2 == 0);
      }
    }
    else {
      DAT_8009a410 = DAT_8009a410 + 1;
      if (600 < DAT_8009a410) {
        DAT_8009a40c = 0;
        goto LAB_8001323c;
      }
    }
    if (uVar3 == 0) {
      VSync(0);
    }
    else {
      FUN_80031f94(1);
    }
    if (1 < DAT_800d5300) goto LAB_80012ff4;
    if (DAT_800d5300 == 0) {
      return DAT_800d5310;
    }
  } while( true );
}


