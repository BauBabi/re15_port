short SsUtKeyOnV(short voice,short vabId,short prog,short tone,short note,short fine,short voll,
                short volr)

{
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  int iVar4;
  undefined1 *puVar5;
  int iVar6;
  
  if (DAT_800b283c == 1) {
    sVar2 = VMANAGER_OBJ_45FC();
    return sVar2;
  }
  DAT_800b283c = 1;
  if (((ushort)voice < 0x18) && (iVar4 = SpuVmVSetUp((int)vabId,(int)prog), iVar4 == 0)) {
    DAT_800b532a = 0x21;
    iVar6 = (int)voll;
    iVar4 = (int)volr;
    DAT_800b5316 = (undefined1)note;
    DAT_800b5317 = (undefined1)fine;
    DAT_800b5320 = (byte)tone;
    DAT_800b5318 = (undefined1)voll;
    if (iVar6 == iVar4) {
      DAT_800b5319 = 0x40;
      sVar2 = VMANAGER_OBJ_441C();
      return sVar2;
    }
    if (iVar4 < iVar6) {
      if (iVar6 == 0) {
        trap(0x1c00);
      }
      if ((iVar6 == -1) && (iVar4 << 6 == -0x80000000)) {
        trap(0x1800);
      }
      DAT_800b5319 = (char)((iVar4 << 6) / iVar6);
      sVar2 = VMANAGER_OBJ_441C();
      return sVar2;
    }
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (iVar6 << 6 == -0x80000000)) {
      trap(0x1800);
    }
    DAT_800b5318 = (undefined1)volr;
    DAT_800b5319 = '\x7f' - (char)((iVar6 << 6) / iVar4);
    puVar5 = (undefined1 *)(((int)((uint)(ushort)prog << 0x10) >> 0xc) + DAT_800b2b28);
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
      iVar4 = (int)voice;
      DAT_800b532e = voice;
      (&DAT_8008f852)[iVar4 * 0x1a] = 0x21;
      (&DAT_8008f85a)[iVar4 * 0x1a] = vabId;
      bVar1 = DAT_800b531b;
      (&DAT_8008f856)[iVar4 * 0x1a] = prog;
      (&DAT_8008f854)[iVar4 * 0x1a] = (ushort)bVar1;
      (&DAT_8008f844)[iVar4 * 0x1a] = DAT_800b532c;
      bVar1 = DAT_800b5320;
      (&DAT_8008f850)[iVar4 * 0x1a] = note;
      (&DAT_8008f85f)[iVar4 * 0x34] = 1;
      (&DAT_8008f846)[iVar4 * 0x1a] = 0;
      (&DAT_8008f858)[iVar4 * 0x1a] = (ushort)bVar1;
      SpuVmDoAllocate();
      if (DAT_800b532c == 0xff) {
        vmNoiseOn(voice & 0xff);
        sVar2 = VMANAGER_OBJ_45EC();
        return sVar2;
      }
      uVar3 = note2pitch2(note,fine);
      SpuVmKeyOnNow(1,uVar3);
      DAT_800b283c = 0;
      return voice;
    }
  }
  DAT_800b283c = 0;
  sVar2 = VMANAGER_OBJ_45FC();
  return sVar2;
}
