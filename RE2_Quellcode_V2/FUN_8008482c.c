/* FUN_8008482c @ 0x8008482c  (Ghidra headless, raw MIPS overlay) */

int FUN_8008482c(uint param_1,short param_2,short param_3,ushort param_4,ushort param_5)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  short sVar12;
  
  iVar11 = 0;
  iVar8 = (int)param_3;
  uVar10 = (uint)param_5;
  iVar9 = (&DAT_800ea250)[param_1 & 0xff] + ((int)(param_1 & 0xff00) >> 8) * 0xb0;
  _SsVmVSetUp((int)param_2,iVar8);
  sVar12 = (short)param_1;
  if (uVar10 == 0) {
    uVar10 = 1;
  }
  if (param_4 == 0) {
    param_4 = 1;
  }
  iVar7 = 0;
  DAT_800dcc46 = sVar12;
  if (DAT_800d7854 != 0) {
    iVar2 = 0;
    do {
      iVar2 = iVar2 >> 0x10;
      if ((((&DAT_800cbce0)[iVar2 * 0x1b] == sVar12) &&
          ((short)(&DAT_800cbce4)[iVar2 * 0x1b] == iVar8)) &&
         ((&DAT_800cbce8)[iVar2 * 0x1b] == param_2)) {
        iVar2 = (uint)*(byte *)(iVar9 + 0x17) * 2 + iVar9;
        uVar3 = (uint)*(short *)(iVar2 + 0x60);
        if ((uVar3 != param_4) && (uVar3 == 0)) {
          *(undefined2 *)(iVar2 + 0x60) = 1;
        }
        sVar1 = (short)iVar7;
        iVar2 = (int)sVar1;
        iVar6 = ((short)(&DAT_800cbce2)[iVar2 * 0x1b] * 0x10 +
                (int)(short)(&DAT_800cbce6)[iVar2 * 0x1b]) * 0x20 + DAT_800d784c;
        uVar5 = (((int)(((int)((int)(short)(&DAT_800cbcd8)[iVar2 * 0x1b] * (uint)param_4) / 0x7f) *
                       (uint)*(byte *)(DAT_800d7848 + 0x18) * 0x3fff) / 0x3f01) *
                 (uint)*(byte *)(iVar8 * 0x10 + DAT_800d7838 + 1) * (uint)*(byte *)(iVar6 + 2)) /
                0x3f01;
        uVar3 = (uVar5 * *(ushort *)(iVar9 + 0x58)) / 0x7f;
        uVar4 = (uint)*(byte *)(iVar6 + 3);
        uVar5 = (uVar5 * *(ushort *)(iVar9 + 0x5a)) / 0x7f;
        if (uVar4 < 0x40) {
          uVar5 = (uVar5 * uVar4) / 0x3f;
        }
        else {
          uVar3 = (uVar3 * (0x7f - uVar4)) / 0x3f;
        }
        uVar4 = (uint)*(byte *)((short)(&DAT_800cbce4)[sVar1 * 0x1b] * 0x10 + DAT_800d7838 + 4);
        if (uVar4 < 0x40) {
          uVar5 = (uVar5 * uVar4) / 0x3f;
        }
        else {
          uVar3 = (uVar3 * (0x7f - uVar4)) / 0x3f;
        }
        uVar4 = uVar10 & 0xff;
        if (uVar4 < 0x40) {
          uVar5 = (uVar5 * uVar4) / 0x3f;
        }
        else {
          uVar3 = (uVar3 * (0x7f - uVar4)) / 0x3f;
        }
        uVar4 = uVar5;
        if ((DAT_800d6c40 == 1) && (uVar4 = uVar3, uVar3 < uVar5)) {
          uVar4 = uVar5;
          uVar3 = uVar5;
        }
        iVar2 = (int)sVar1;
        (&DAT_800dccb0)[iVar2 * 8] = (short)((uVar3 * uVar3) / 0x3fff);
        (&DAT_800dccb2)[iVar2 * 8] = (short)((uVar4 * uVar4) / 0x3fff);
        iVar11 = iVar11 + 1;
        (&DAT_800d4c80)[iVar2] = (&DAT_800d4c80)[iVar2] | 3;
      }
      iVar7 = iVar7 + 1;
      iVar2 = iVar7 * 0x10000;
    } while (iVar7 * 0x10000 >> 0x10 < (int)(uint)DAT_800d7854);
  }
  return iVar11;
}


