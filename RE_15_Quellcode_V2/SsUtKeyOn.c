short SsUtKeyOn(short param_1,short param_2,short param_3,short param_4,short param_5,short param_6,
               short param_7)

{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  int iVar4;
  undefined1 *puVar5;
  uint uVar6;
  int iVar7;
  
  if (DAT_800b283c == 1) {
    sVar2 = VMANAGER_OBJ_406C();
    return sVar2;
  }
  DAT_800b283c = 1;
  iVar4 = SpuVmVSetUp((int)param_1,(int)param_2);
  if (iVar4 == 0) {
    DAT_800b532a = 0x21;
    iVar7 = (int)param_6;
    iVar4 = (int)param_7;
    DAT_800b5316 = (undefined1)param_4;
    DAT_800b5317 = (undefined1)param_5;
    DAT_800b5320 = (byte)param_3;
    DAT_800b5318 = (undefined1)param_6;
    if (iVar7 == iVar4) {
      DAT_800b5319 = 0x40;
      sVar2 = VMANAGER_OBJ_3E74();
      return sVar2;
    }
    if (iVar4 < iVar7) {
      if (iVar7 == 0) {
        trap(0x1c00);
      }
      if ((iVar7 == -1) && (iVar4 << 6 == -0x80000000)) {
        trap(0x1800);
      }
      DAT_800b5319 = (char)((iVar4 << 6) / iVar7);
      sVar2 = VMANAGER_OBJ_3E74();
      return sVar2;
    }
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (iVar7 << 6 == -0x80000000)) {
      trap(0x1800);
    }
    DAT_800b5318 = (undefined1)param_7;
    DAT_800b5319 = '\x7f' - (char)((iVar7 << 6) / iVar4);
    puVar5 = (undefined1 *)(((int)((uint)(ushort)param_2 << 0x10) >> 0xc) + DAT_800b2b28);
    DAT_800b531e = puVar5[1];
    DAT_800b531f = puVar5[4];
    DAT_800b5314 = *puVar5;
    puVar5 = (undefined1 *)(((uint)DAT_800b5320 + (uint)DAT_800b531b * 0x10) * 0x20 + DAT_800b2b3c);
    DAT_800b5323 = *puVar5;
    DAT_800b532c = *(short *)(puVar5 + 0x16);
    DAT_800b5321 = puVar5[2];
    DAT_800b5322 = puVar5[3];
    DAT_800b5324 = puVar5[4];
    DAT_800b5325 = puVar5[5];
    DAT_800b5328 = puVar5[1];
    DAT_800b5326 = puVar5[6];
    DAT_800b5327 = puVar5[7];
    if (DAT_800b532c != 0) {
      uVar6 = SpuVmAlloc();
      uVar6 = uVar6 & 0xff;
      if (uVar6 != DAT_800b52a8) {
        DAT_800b532e = (short)uVar6;
        (&DAT_8008f852)[uVar6 * 0x1a] = 0x21;
        (&DAT_8008f85a)[uVar6 * 0x1a] = param_1;
        bVar1 = DAT_800b531b;
        (&DAT_8008f856)[uVar6 * 0x1a] = param_2;
        (&DAT_8008f854)[uVar6 * 0x1a] = (ushort)bVar1;
        (&DAT_8008f844)[uVar6 * 0x1a] = DAT_800b532c;
        bVar1 = DAT_800b5320;
        (&DAT_8008f850)[uVar6 * 0x1a] = param_4;
        (&DAT_8008f85f)[uVar6 * 0x34] = 1;
        (&DAT_8008f846)[uVar6 * 0x1a] = 0;
        (&DAT_8008f858)[uVar6 * 0x1a] = (ushort)bVar1;
        SpuVmDoAllocate();
        if (DAT_800b532c == 0xff) {
          vmNoiseOn(uVar6);
          sVar2 = VMANAGER_OBJ_4060();
          return sVar2;
        }
        uVar3 = note2pitch2(param_4,param_5);
        SpuVmKeyOnNow(1,uVar3);
        DAT_800b283c = 0;
        return (short)uVar6;
      }
    }
  }
  DAT_800b283c = 0;
  sVar2 = VMANAGER_OBJ_406C();
  return sVar2;
}
