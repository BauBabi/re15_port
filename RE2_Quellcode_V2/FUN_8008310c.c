/* FUN_8008310c @ 0x8008310c  (Ghidra headless, raw MIPS overlay) */

void FUN_8008310c(uint param_1)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  ushort uVar6;
  uint uVar7;
  uint uVar8;
  
  iVar3 = (&DAT_800ea250)[DAT_800dcc46 & 0xff] + (uint)(DAT_800dcc46 >> 8) * 0xb0;
  uVar5 = (uint)DAT_800dcc3e;
  uVar7 = ((((uint)*(ushort *)(iVar3 + 0x58) * 0x81 * (uint)DAT_800dcc3a) / 0x7f) *
          (uint)DAT_800dcc3d) / 0x7f;
  uVar8 = ((((uint)*(ushort *)(iVar3 + 0x5a) * 0x81 * (uint)DAT_800dcc3a) / 0x7f) *
          (uint)DAT_800dcc3d) / 0x7f;
  if (uVar5 < 0x40) {
    uVar8 = (uVar8 * uVar5) / 0x3f;
  }
  else {
    uVar7 = (uVar7 * (0x7f - uVar5)) / 0x3f;
  }
  uVar5 = (uint)DAT_800dcc3b;
  if (uVar5 < 0x40) {
    uVar8 = (uVar8 * uVar5) / 0x3f;
  }
  else {
    uVar7 = (uVar7 * (0x7f - uVar5)) / 0x3f;
  }
  uVar5 = (uint)DAT_800dcc35;
  if (uVar5 < 0x40) {
    uVar8 = (uVar5 * uVar8) / 0x3f;
  }
  else {
    uVar7 = (uVar7 * (0x7f - uVar5)) / 0x3f;
  }
  uVar5 = uVar8;
  if ((DAT_800d6c40 == 1) && (uVar5 = uVar7, uVar7 < uVar8)) {
    uVar5 = uVar8;
    uVar7 = uVar8;
  }
  SpuSetNoiseClock((uint)DAT_800dcc32 - (uint)DAT_800dcc40 & 0x3f);
  uVar8 = param_1 & 0xff;
  (&DAT_800dccb2)[uVar8 * 8] = (short)uVar5;
  bVar1 = (&DAT_800d4c80)[uVar8];
  (&DAT_800dccb0)[uVar8 * 8] = (short)uVar7;
  (&DAT_800d4c80)[uVar8] = bVar1 | 3;
  bVar1 = DAT_800d7854;
  if (uVar8 < 0x10) {
    uVar6 = (ushort)(1 << (param_1 & 0x1f));
    uVar7 = 0;
  }
  else {
    uVar6 = 0;
    uVar7 = 1 << (uVar8 - 0x10 & 0x1f);
  }
  (&DAT_800cbcd4)[(param_1 & 0xff) * 0x1b] = 10;
  iVar3 = 0;
  if (bVar1 != 0) {
    iVar4 = 0;
    do {
      iVar4 = (iVar4 >> 0x10) * 0x36;
      iVar3 = iVar3 + 1;
      (&DAT_800cbced)[iVar4] = (&DAT_800cbced)[iVar4] & 1;
      iVar4 = iVar3 * 0x10000;
    } while (iVar3 * 0x10000 >> 0x10 < (int)(uint)DAT_800d7854);
  }
  (&DAT_800cbced)[(param_1 & 0xff) * 0x36] = 2;
  DAT_800c440c = DAT_800c440c | uVar6;
  uVar2 = (ushort)uVar7;
  DAT_800c440e = DAT_800c440e | uVar2;
  DAT_800eaea0 = DAT_800eaea0 & ~DAT_800c440c;
  DAT_800eaea2 = DAT_800eaea2 & ~DAT_800c440e;
  if ((DAT_800dcc44 & 4) == 0) {
    DAT_800c4412 = DAT_800c4412 & ~uVar6;
    DAT_800c4414 = DAT_800c4414 & ~uVar2;
  }
  else {
    DAT_800c4412 = DAT_800c4412 | uVar6;
    DAT_800c4414 = DAT_800c4414 | uVar2;
  }
  SpuSetNoiseVoice(1,(uVar7 & 0xff) << 0x10 | (int)(short)uVar6);
  return;
}


