void SePitchBend(uint param_1,short param_2)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = param_1 & 0xff;
  if (uVar3 < 0x18) {
    DAT_800b531b = *(byte *)(&DAT_8008f854 + uVar3 * 0x1a);
    DAT_800b5320 = *(byte *)(&DAT_8008f858 + uVar3 * 0x1a);
    DAT_800b532e = (ushort)param_1 & 0xff;
    iVar4 = (int)param_2;
    iVar2 = ((uint)DAT_800b5320 + (uint)DAT_800b531b * 0x10) * 0x20;
    if (-1 < iVar4) {
      VMANAGER_OBJ_12CC((uint)(ushort)(&DAT_8008f850)[uVar3 * 0x1a] +
                        (int)(iVar4 * (uint)*(byte *)(iVar2 + DAT_800b2b3c + 0xd)) / 0x7f);
      return;
    }
    iVar2 = (int)(iVar4 * (uint)*(byte *)(iVar2 + DAT_800b2b3c + 0xc)) / 0x7f;
    uVar1 = note2pitch2(((uint)(ushort)(&DAT_8008f850)[uVar3 * 0x1a] + iVar2) - 1 & 0xffff,
                        iVar2 + 0x7fU & 0xffff);
    (&DAT_8008f6b0)[uVar3 * 8] = uVar1;
    (&DAT_8008f82c)[param_1 & 0xff] = (&DAT_8008f82c)[param_1 & 0xff] | 4;
  }
  return;
}
