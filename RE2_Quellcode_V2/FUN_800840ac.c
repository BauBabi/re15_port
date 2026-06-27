/* FUN_800840ac @ 0x800840ac  (Ghidra headless, raw MIPS overlay) */

int FUN_800840ac(uint param_1,undefined2 param_2,undefined2 param_3)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (&DAT_800ea250)[param_1 & 0xff] + ((int)(param_1 & 0xff00) >> 8) * 0xb0;
  *(undefined2 *)(iVar8 + 0x58) = param_2;
  *(undefined2 *)(iVar8 + 0x5a) = param_3;
  if (0x7e < *(ushort *)(iVar8 + 0x58)) {
    *(undefined2 *)(iVar8 + 0x58) = 0x7f;
  }
  if (0x7e < *(ushort *)(iVar8 + 0x5a)) {
    *(undefined2 *)(iVar8 + 0x5a) = 0x7f;
  }
  iVar7 = 0;
  if (DAT_800d7854 != 0) {
    iVar2 = 0;
    do {
      iVar2 = iVar2 >> 0x10;
      if ((&DAT_800cbce0)[iVar2 * 0x1b] == (short)param_1) {
        _SsVmVSetUp((int)(short)(&DAT_800cbce8)[iVar2 * 0x1b],
                    (int)(short)(&DAT_800cbce2)[iVar2 * 0x1b]);
        iVar6 = ((short)(&DAT_800cbce2)[iVar2 * 0x1b] * 0x10 +
                (int)(short)(&DAT_800cbce6)[iVar2 * 0x1b]) * 0x20 + DAT_800d784c;
        uVar5 = (((int)((uint)*(byte *)(DAT_800d7848 + 0x18) *
                       (((int)(short)(&DAT_800cbcd8)[iVar2 * 0x1b] *
                        (int)*(short *)((short)(&DAT_800cbcdc)[iVar2 * 0x1b] * 2 + iVar8 + 0x60)) /
                       0x7f) * 0x3fff) / 0x3f01) *
                 (uint)*(byte *)((short)(&DAT_800cbce4)[iVar2 * 0x1b] * 0x10 + DAT_800d7838 + 1) *
                (uint)*(byte *)(iVar6 + 2)) / 0x3f01;
        uVar4 = (uVar5 * *(ushort *)(iVar8 + 0x58)) / 0x7f;
        uVar3 = (uint)*(byte *)(iVar6 + 3);
        uVar5 = (uVar5 * *(ushort *)(iVar8 + 0x5a)) / 0x7f;
        if (uVar3 < 0x40) {
          uVar5 = (uVar5 * uVar3) / 0x3f;
        }
        else {
          uVar4 = (uVar4 * (0x7f - uVar3)) / 0x3f;
        }
        sVar1 = (short)iVar7;
        uVar3 = (uint)*(byte *)((short)(&DAT_800cbce4)[sVar1 * 0x1b] * 0x10 + DAT_800d7838 + 4);
        if (uVar3 < 0x40) {
          uVar5 = (uint)((ulonglong)((longlong)(int)((uVar5 & 0xffff) * uVar3) * 0x82082083) >> 0x25
                        );
        }
        else {
          uVar4 = (int)((uVar4 & 0xffff) * (0x7f - uVar3)) / 0x3f;
        }
        uVar3 = (uint)(byte)(&DAT_800cbcda)[sVar1 * 0x36];
        if (uVar3 < 0x40) {
          uVar5 = (uint)((ulonglong)((longlong)(int)((uVar5 & 0xffff) * uVar3) * 0x82082083) >> 0x25
                        );
        }
        else {
          uVar4 = (int)((uVar4 & 0xffff) * (0x7f - uVar3)) / 0x3f;
        }
        uVar3 = uVar5;
        if ((DAT_800d6c40 == 1) && (uVar3 = uVar4, (uVar4 & 0xffff) < (uVar5 & 0xffff))) {
          uVar4 = uVar5;
          uVar3 = uVar5;
        }
        iVar2 = (int)sVar1;
        (&DAT_800dccb2)[iVar2 * 8] = (short)((int)((uVar3 & 0xffff) * (uVar3 & 0xffff)) / 0x3fff);
        (&DAT_800d4c80)[iVar2] = (&DAT_800d4c80)[iVar2] | 3;
        (&DAT_800dccb0)[iVar2 * 8] = (short)((int)((uVar4 & 0xffff) * (uVar4 & 0xffff)) / 0x3fff);
      }
      iVar7 = iVar7 + 1;
      iVar2 = iVar7 * 0x10000;
    } while (iVar7 * 0x10000 >> 0x10 < (int)(uint)DAT_800d7854);
  }
  return (int)(short)param_1;
}


