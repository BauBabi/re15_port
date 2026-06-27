int SpuVmSetVol(uint param_1,short param_2,short param_3,uint param_4,ushort param_5)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  byte bVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  short sVar11;
  
  iVar10 = 0;
  iVar8 = (int)param_3;
  iVar9 = ((int)(param_1 & 0xff00) >> 8) * 0xac + (&DAT_800bb500)[param_1 & 0xff];
  SpuVmVSetUp((int)param_2,iVar8);
  sVar11 = (short)param_1;
  bVar7 = 0;
  DAT_800b532a = sVar11;
  if (DAT_800b52a8 != 0) {
    uVar1 = 0;
    do {
      if ((((&DAT_8008f852)[uVar1 * 0x1a] == sVar11) &&
          ((short)(&DAT_8008f856)[uVar1 * 0x1a] == iVar8)) &&
         ((&DAT_8008f85a)[uVar1 * 0x1a] == param_2)) {
        iVar4 = (uint)*(byte *)(iVar9 + 0x12) * 2 + iVar9;
        uVar1 = (uint)*(short *)(iVar4 + 0x4e);
        if ((uVar1 != (param_4 & 0xffff)) && (uVar1 == 0)) {
          *(undefined2 *)(iVar4 + 0x4e) = 1;
        }
        uVar6 = (((int)(((int)((int)(short)(&DAT_8008f84c)[(uint)bVar7 * 0x1a] * (param_4 & 0xffff))
                        / 0x7f) * (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) *
                 (uint)*(byte *)(iVar8 * 0x10 + DAT_800b2b28 + 1) *
                (uint)*(byte *)((iVar8 * 0x10 + (int)(short)(&DAT_8008f858)[(uint)bVar7 * 0x1a]) *
                                0x20 + DAT_800b2b3c + 2)) / 0x3f01;
        uVar5 = (uint)*(byte *)((short)(&DAT_8008f858)[(uint)bVar7 * 0x1a] * 0x20 + DAT_800b2b3c + 3
                               );
        uVar1 = (uVar6 * *(ushort *)(iVar9 + 0x74)) / 0x7f;
        uVar6 = (uVar6 * *(ushort *)(iVar9 + 0x76)) / 0x7f;
        if (uVar5 < 0x40) {
          iVar8 = VMANAGER_OBJ_3AF4((uVar6 * uVar5) / 0x3f,uVar5,uVar1);
          return iVar8;
        }
        uVar2 = (uint)*(byte *)((short)(&DAT_8008f854)[(uint)bVar7 * 0x1a] * 0x10 + DAT_800b2b28 + 4
                               );
        if (uVar2 < 0x40) {
          iVar8 = VMANAGER_OBJ_3B90((uVar6 * uVar2) / 0x3f);
          return iVar8;
        }
        uVar3 = param_5 & 0xff;
        if (uVar3 < 0x40) {
          iVar8 = VMANAGER_OBJ_3BF4((uVar6 * uVar3) / 0x3f);
          return iVar8;
        }
        uVar2 = (((((uVar1 * (0x7f - uVar5)) / 0x3f) * (0x7f - uVar2)) / 0x3f) * (0x7f - uVar3)) /
                0x3f;
        uVar1 = uVar6;
        if ((DAT_800b2678 == 1) && (uVar1 = uVar2, uVar2 < uVar6)) {
          iVar8 = VMANAGER_OBJ_3C20(uVar6,uVar5,uVar6);
          return iVar8;
        }
        uVar5 = (uint)bVar7;
        (&DAT_8008f6ac)[uVar5 * 8] = (short)((uVar2 * uVar2) / 0x3fff);
        (&DAT_8008f6ae)[uVar5 * 8] = (short)((uVar1 * uVar1) / 0x3fff);
        iVar10 = iVar10 + 1;
        (&DAT_8008f82c)[uVar5] = (&DAT_8008f82c)[uVar5] | 3;
      }
      bVar7 = bVar7 + 1;
      uVar1 = (uint)bVar7;
    } while (bVar7 < DAT_800b52a8);
  }
  return iVar10;
}
