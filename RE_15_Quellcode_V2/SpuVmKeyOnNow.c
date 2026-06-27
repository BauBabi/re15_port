void SpuVmKeyOnNow(undefined4 param_1,undefined2 param_2)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  
  uVar8 = ((((uint)DAT_800b5318 * (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) *
           (uint)DAT_800b531e * (uint)DAT_800b5321) / 0x3f01;
  uVar6 = DAT_800b532a & 0xff;
  uVar7 = uVar6 * 4;
  uVar2 = (uint)DAT_800b532a << 0x10;
  iVar3 = (uVar2 >> 0x18) * 0xac + (&DAT_800bb500)[uVar6];
  uVar6 = uVar8;
  if ((int)uVar2 >> 0x10 != 0x21) {
    uVar2 = uVar8 * *(ushort *)(iVar3 + 0x74);
    uVar6 = uVar8 * *(ushort *)(iVar3 + 0x76);
    uVar7 = uVar2 - (int)((ulonglong)uVar2 * 0x2040811 >> 0x20) >> 1;
    uVar8 = uVar2 / 0x7f;
    uVar6 = uVar6 / 0x7f;
  }
  uVar2 = (uint)DAT_800b5322;
  if (uVar2 < 0x40) {
    VMANAGER_OBJ_434(uVar7,(uVar6 * uVar2) / 0x3f);
    return;
  }
  uVar4 = (uint)DAT_800b531f;
  if (uVar4 < 0x40) {
    VMANAGER_OBJ_4A4(uVar7,(uVar6 * uVar4) / 0x3f);
    return;
  }
  uVar5 = (uint)DAT_800b5319;
  if (0x3f < uVar5) {
    uVar8 = (((((uVar8 * (0x7f - uVar2)) / 0x3f) * (0x7f - uVar4)) / 0x3f) * (0x7f - uVar5)) / 0x3f;
    uVar2 = uVar6;
    if ((DAT_800b2678 == 1) && (uVar2 = uVar8, uVar8 < uVar6)) {
      VMANAGER_OBJ_540(uVar7,uVar6,uVar6);
      return;
    }
    uVar6 = (int)DAT_800b532e & 0x1fff;
    (&DAT_8008f6b0)[uVar6 * 8] = param_2;
    (&DAT_8008f6ac)[uVar6 * 8] = (short)((uVar8 * uVar8) / 0x3fff);
    (&DAT_8008f6ae)[uVar6 * 8] = (short)((uVar2 * uVar2) / 0x3fff);
    (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 7;
    (&DAT_8008f848)[DAT_800b532e * 0x1a] = param_2;
    (&DAT_8008f85f)[DAT_800b532e * 0x34] = 1;
    uVar6 = (uint)DAT_800b532e;
    if ((int)uVar6 < 0x10) {
      VMANAGER_OBJ_670(&DAT_800b532e,0,1 << (uVar6 & 0x1f));
      return;
    }
    uVar1 = (ushort)(1 << (uVar6 - 0x10 & 0x1f));
    if ((DAT_800b5328 & 4) == 0) {
      DAT_8008ff90 = DAT_8008ff90 & ~uVar1;
      DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
      DAT_8008ff88 = uVar1 | DAT_8008ff88;
      DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
      return;
    }
    DAT_8008ff90 = uVar1 | DAT_8008ff90;
    VMANAGER_OBJ_6E8();
    return;
  }
  VMANAGER_OBJ_514(uVar7,(uVar6 * uVar5) / 0x3f);
  return;
}
