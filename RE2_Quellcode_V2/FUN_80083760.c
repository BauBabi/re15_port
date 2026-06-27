/* FUN_80083760 @ 0x80083760  (Ghidra headless, raw MIPS overlay) */

void FUN_80083760(undefined4 param_1,undefined2 param_2)

{
  uint uVar1;
  int iVar2;
  ushort uVar3;
  ushort uVar4;
  uint uVar5;
  uint uVar6;
  
  uVar5 = ((((uint)DAT_800dcc34 * (uint)*(byte *)(DAT_800d7848 + 0x18) * 0x3fff) / 0x3f01) *
           (uint)DAT_800dcc3a * (uint)DAT_800dcc3d) / 0x3f01;
  uVar1 = (uint)DAT_800dcc46 << 0x10;
  iVar2 = (&DAT_800ea250)[DAT_800dcc46 & 0xff] + (uVar1 >> 0x18) * 0xb0;
  uVar6 = uVar5;
  if ((int)uVar1 >> 0x10 != 0x21) {
    uVar6 = (uVar5 * *(ushort *)(iVar2 + 0x58)) / 0x7f;
    uVar5 = (uVar5 * *(ushort *)(iVar2 + 0x5a)) / 0x7f;
  }
  uVar1 = (uint)DAT_800dcc3e;
  if (uVar1 < 0x40) {
    uVar5 = (uVar5 * uVar1) / 0x3f;
  }
  else {
    uVar6 = (uVar6 * (0x7f - uVar1)) / 0x3f;
  }
  uVar1 = (uint)DAT_800dcc3b;
  if (uVar1 < 0x40) {
    uVar5 = (uVar5 * uVar1) / 0x3f;
  }
  else {
    uVar6 = (uVar6 * (0x7f - uVar1)) / 0x3f;
  }
  uVar1 = (uint)DAT_800dcc35;
  if (uVar1 < 0x40) {
    uVar5 = (uVar5 * uVar1) / 0x3f;
  }
  else {
    uVar6 = (uVar6 * (0x7f - uVar1)) / 0x3f;
  }
  uVar1 = uVar5;
  if ((DAT_800d6c40 == 1) && (uVar1 = uVar6, uVar6 < uVar5)) {
    uVar1 = uVar5;
    uVar6 = uVar5;
  }
  if (DAT_800dcc46 != 0x21) {
    uVar6 = (uVar6 * uVar6) / 0x3fff;
    uVar1 = (uVar1 * uVar1) / 0x3fff;
  }
  uVar5 = (int)DAT_800dcc4a & 0x1fff;
  (&DAT_800dccb4)[uVar5 * 8] = param_2;
  (&DAT_800dccb0)[uVar5 * 8] = (short)uVar6;
  (&DAT_800dccb2)[uVar5 * 8] = (short)uVar1;
  (&DAT_800d4c80)[DAT_800dcc4a] = (&DAT_800d4c80)[DAT_800dcc4a] | 7;
  (&DAT_800cbcd4)[DAT_800dcc4a * 0x1b] = param_2;
  uVar6 = (uint)DAT_800dcc4a;
  if ((int)uVar6 < 0x10) {
    uVar4 = (ushort)(1 << (uVar6 & 0x1f));
    uVar3 = 0;
  }
  else {
    uVar4 = 0;
    uVar3 = (ushort)(1 << (uVar6 - 0x10 & 0x1f));
  }
  if ((DAT_800dcc44 & 4) == 0) {
    DAT_800c4412 = DAT_800c4412 & ~uVar4;
    DAT_800c4414 = DAT_800c4414 & ~uVar3;
  }
  else {
    DAT_800c4412 = DAT_800c4412 | uVar4;
    DAT_800c4414 = DAT_800c4414 | uVar3;
  }
  DAT_800c440c = DAT_800c440c | uVar4;
  DAT_800eaea0 = DAT_800eaea0 & ~DAT_800c440c;
  DAT_800c440e = DAT_800c440e | uVar3;
  DAT_800eaea2 = DAT_800eaea2 & ~DAT_800c440e;
  return;
}


