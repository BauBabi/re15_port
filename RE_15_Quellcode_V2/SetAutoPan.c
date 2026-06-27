void SetAutoPan(short param_1)

{
  byte bVar1;
  ushort uVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  
  iVar4 = (int)param_1;
  if ((&DAT_8008f870)[iVar4 * 0x1a] != 0) {
    uVar2 = (&DAT_8008f872)[iVar4 * 0x1a];
    (&DAT_8008f872)[iVar4 * 0x1a] = uVar2 - 1;
    if (0 < (int)((uint)uVar2 << 0x10)) {
      return;
    }
    (&DAT_8008f872)[iVar4 * 0x1a] = (&DAT_8008f870)[iVar4 * 0x1a];
  }
  sVar3 = (&DAT_8008f874)[iVar4 * 0x1a] + (&DAT_8008f86e)[iVar4 * 0x1a];
  (&DAT_8008f874)[iVar4 * 0x1a] = sVar3;
  if ((short)(&DAT_8008f86e)[iVar4 * 0x1a] < 1) {
    if (((short)(&DAT_8008f86e)[iVar4 * 0x1a] < 0) &&
       (sVar3 <= *(short *)(&DAT_8008f876 + iVar4 * 0x34))) {
      (&DAT_8008f874)[iVar4 * 0x1a] = *(short *)(&DAT_8008f876 + iVar4 * 0x34);
      (&DAT_8008f86c)[iVar4 * 0x1a] = 0;
    }
  }
  else if ((int)*(short *)(&DAT_8008f876 + iVar4 * 0x34) <= (int)sVar3) {
    VMANAGER_OBJ_1AF4((int)*(short *)(&DAT_8008f876 + iVar4 * 0x34));
    return;
  }
  DAT_800b5319 = *(byte *)(&DAT_8008f874 + param_1 * 0x1a);
  uVar8 = (uint)DAT_800b5319;
  uVar6 = (uint)DAT_800b5322;
  uVar5 = ((((uint)DAT_800b5318 * (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) *
           (uint)DAT_800b531e * (uint)DAT_800b5321) / 0x3f01;
  if (uVar6 < 0x40) {
    VMANAGER_OBJ_1C04(uVar6,uVar5,uVar5 * uVar6 >> 6);
    return;
  }
  uVar7 = uVar5 * (0x7f - uVar6) >> 6;
  uVar6 = (uint)DAT_800b531f;
  if (uVar6 < 0x40) {
    VMANAGER_OBJ_1C5C(uVar8,uVar7,(uVar5 & 0xffff) * uVar6 >> 6);
    return;
  }
  uVar6 = (uVar7 & 0xffff) * (0x7f - uVar6);
  if ((int)uVar6 < 0) {
    uVar6 = uVar6 + 0x3f;
  }
  uVar6 = uVar6 >> 6;
  if (uVar8 < 0x40) {
    uVar5 = (uVar5 & 0xffff) * uVar8 >> 6;
  }
  else {
    uVar8 = (uVar6 & 0xffff) * (0x7f - uVar8);
    uVar6 = uVar8 >> 6;
    if ((int)uVar8 < 0) {
      uVar6 = uVar8 + 0x3f >> 6;
    }
  }
  uVar8 = uVar5;
  if ((DAT_800b2678 == 1) && (uVar8 = uVar6, (uVar6 & 0xffff) < (uVar5 & 0xffff))) {
    VMANAGER_OBJ_1CDC(iVar4 << 0x13,uVar5);
    return;
  }
  iVar4 = (iVar4 << 0x13) >> 0xf;
  *(short *)((int)&DAT_8008f6ae + iVar4) = (short)uVar8;
  bVar1 = (&DAT_8008f82c)[param_1];
  *(short *)((int)&DAT_8008f6ac + iVar4) = (short)uVar6;
  (&DAT_8008f82c)[param_1] = bVar1 | 3;
  return;
}
