uint SpuVmAlloc(void)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  byte bVar4;
  short sVar5;
  byte bVar6;
  char cVar7;
  uint uVar8;
  
  bVar1 = DAT_800b52a8;
  cVar7 = '\0';
  sVar5 = 0;
  bVar6 = 99;
  uVar8 = (uint)DAT_800b5323;
  bVar4 = 0;
  if (DAT_800b52a8 != 0) {
    uVar2 = 0;
    do {
      if (((&DAT_8008f85f)[uVar2 * 0x34] == '\0') && ((&DAT_8008f84a)[uVar2 * 0x1a] == 0)) {
        uVar8 = VMANAGER_OBJ_164();
        return uVar8;
      }
      uVar2 = (uint)bVar4;
      uVar3 = (uint)(short)(&DAT_8008f85c)[uVar2 * 0x1a];
      if ((int)uVar3 < (int)uVar8) {
        uVar8 = VMANAGER_OBJ_148(uVar8,uVar3);
        return uVar8;
      }
      if (uVar3 == uVar8) {
        cVar7 = cVar7 + '\x01';
        if ((&DAT_8008f84a)[uVar2 * 0x1a] != -1) {
          uVar8 = VMANAGER_OBJ_144();
          return uVar8;
        }
        if (sVar5 < (short)(&DAT_8008f846)[uVar2 * 0x1a]) {
          sVar5 = (&DAT_8008f846)[uVar2 * 0x1a];
          bVar6 = bVar4;
        }
      }
      bVar4 = bVar4 + 1;
      uVar2 = (uint)bVar4;
    } while (bVar4 < DAT_800b52a8);
  }
  if (cVar7 == '\0') {
    bVar6 = DAT_800b52a8;
  }
  if (bVar6 < DAT_800b52a8) {
    bVar4 = 0;
    if (DAT_800b52a8 != 0) {
      uVar8 = 0;
      do {
        bVar4 = bVar4 + 1;
        (&DAT_8008f846)[uVar8 * 0x1a] = (&DAT_8008f846)[uVar8 * 0x1a] + 1;
        uVar8 = (uint)bVar4;
      } while (bVar4 < bVar1);
    }
    uVar8 = (uint)bVar6;
    (&DAT_8008f846)[uVar8 * 0x1a] = 0;
    (&DAT_8008f85c)[uVar8 * 0x1a] = (ushort)DAT_800b5323;
    if ((&DAT_8008f85f)[uVar8 * 0x34] == '\x02') {
      SpuSetNoiseVoice(0,0xffffff);
    }
  }
  return (uint)bVar6;
}
