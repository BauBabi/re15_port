void SetAutoVol(short param_1)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  
  iVar4 = (int)param_1;
  if ((&DAT_8008f864)[iVar4 * 0x1a] != 0) {
    uVar2 = (&DAT_8008f866)[iVar4 * 0x1a];
    (&DAT_8008f866)[iVar4 * 0x1a] = uVar2 - 1;
    if (0 < (int)((uint)uVar2 << 0x10)) {
      return;
    }
    (&DAT_8008f866)[iVar4 * 0x1a] = (&DAT_8008f864)[iVar4 * 0x1a];
  }
  sVar3 = (&DAT_8008f868)[iVar4 * 0x1a] + (&DAT_8008f862)[iVar4 * 0x1a];
  (&DAT_8008f868)[iVar4 * 0x1a] = sVar3;
  if ((short)(&DAT_8008f862)[iVar4 * 0x1a] < 1) {
    if (((short)(&DAT_8008f862)[iVar4 * 0x1a] < 0) &&
       (sVar3 <= *(short *)(&DAT_8008f86a + iVar4 * 0x34))) {
      (&DAT_8008f868)[iVar4 * 0x1a] = *(short *)(&DAT_8008f86a + iVar4 * 0x34);
      (&DAT_8008f860)[iVar4 * 0x1a] = 0;
    }
  }
  else if ((int)*(short *)(&DAT_8008f86a + iVar4 * 0x34) <= (int)sVar3) {
    VMANAGER_OBJ_15F4((int)*(short *)(&DAT_8008f86a + iVar4 * 0x34));
    return;
  }
  DAT_800b5318 = (undefined1)(&DAT_8008f868)[param_1 * 0x1a];
  uVar6 = (uint)DAT_800b5322;
  uVar5 = (((int)((int)(short)(&DAT_8008f868)[param_1 * 0x1a] *
                 (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) * (uint)DAT_800b531e *
          (uint)DAT_800b5321) / 0x3f01;
  if (0x3f < uVar6) {
    uVar7 = uVar5 * (0x7f - uVar6) >> 6;
    uVar6 = (uint)DAT_800b531f;
    if (uVar6 < 0x40) {
      uVar5 = (uVar5 & 0xffff) * uVar6 >> 6;
    }
    else {
      uVar6 = (uVar7 & 0xffff) * (0x7f - uVar6);
      uVar7 = uVar6 >> 6;
      if ((int)uVar6 < 0) {
        uVar7 = uVar6 + 0x3f >> 6;
      }
    }
    uVar6 = (uint)DAT_800b5319;
    if (uVar6 < 0x40) {
      uVar5 = (uVar5 & 0xffff) * uVar6 >> 6;
    }
    else {
      uVar6 = (uVar7 & 0xffff) * (0x7f - uVar6);
      uVar7 = uVar6 >> 6;
      if ((int)uVar6 < 0) {
        uVar7 = uVar6 + 0x3f >> 6;
      }
    }
    uVar6 = uVar5;
    if ((DAT_800b2678 == 1) && (uVar6 = uVar7, (uVar7 & 0xffff) < (uVar5 & 0xffff))) {
      VMANAGER_OBJ_17E8(iVar4 << 0x13,uVar5);
      return;
    }
    iVar4 = (iVar4 << 0x13) >> 0xf;
    *(short *)((int)&DAT_8008f6ae + iVar4) = (short)uVar6;
    bVar1 = (&DAT_8008f82c)[param_1];
    *(short *)((int)&DAT_8008f6ac + iVar4) = (short)uVar7;
    (&DAT_8008f82c)[param_1] = bVar1 | 3;
    return;
  }
  VMANAGER_OBJ_1704(uVar6,uVar5,uVar5 * uVar6 >> 6);
  return;
}
