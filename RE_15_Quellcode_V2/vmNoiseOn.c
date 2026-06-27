void vmNoiseOn(uint param_1)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  
  iVar3 = (uint)(DAT_800b532a >> 8) * 0xac + (&DAT_800bb500)[DAT_800b532a & 0xff];
  uVar7 = (uint)DAT_800b5322;
  uVar8 = ((((uint)*(ushort *)(iVar3 + 0x76) * 0x81 * (uint)DAT_800b531e) / 0x7f) *
          (uint)DAT_800b5321) / 0x7f;
  uVar5 = ((((uint)*(ushort *)(iVar3 + 0x74) * 0x81 * (uint)DAT_800b531e) / 0x7f) *
          (uint)DAT_800b5321) / 0x7f;
  if (uVar7 < 0x40) {
    VMANAGER_OBJ_AF0(uVar5,uVar5,uVar7,(uVar8 * uVar7) / 0x3f);
    return;
  }
  if (DAT_800b531f < 0x40) {
    VMANAGER_OBJ_B60();
    return;
  }
  if (DAT_800b5319 < 0x40) {
    VMANAGER_OBJ_BD0();
    return;
  }
  uVar6 = (((((uVar5 * (0x7f - uVar7)) / 0x3f) * (0x7f - (uint)DAT_800b531f)) / 0x3f) *
          (0x7f - (uint)DAT_800b5319)) / 0x3f;
  uVar7 = uVar8;
  if ((DAT_800b2678 == 1) && (uVar7 = uVar6, uVar6 < uVar8)) {
    VMANAGER_OBJ_BF8(uVar5,uVar8);
    return;
  }
  SPU_CTRL_REG_CPUCNT =
       SPU_CTRL_REG_CPUCNT & 0xc0ff | ((ushort)DAT_800b5316 - (ushort)DAT_800b5324 & 0x3f) << 8;
  uVar5 = param_1 & 0xff;
  (&DAT_8008f6ae)[uVar5 * 8] = (short)uVar7;
  bVar1 = (&DAT_8008f82c)[uVar5];
  (&DAT_8008f6ac)[uVar5 * 8] = (short)uVar6;
  (&DAT_8008f82c)[uVar5] = bVar1 | 3;
  bVar1 = DAT_800b52a8;
  if (uVar5 < 0x10) {
    VMANAGER_OBJ_C88(uVar5,uVar6,0,1 << (param_1 & 0x1f));
    return;
  }
  (&DAT_8008f848)[(param_1 & 0xff) * 0x1a] = 10;
  iVar3 = 0;
  if (bVar1 != 0) {
    iVar4 = 0;
    do {
      iVar4 = (iVar4 >> 0x10) * 0x34;
      iVar3 = iVar3 + 1;
      (&DAT_8008f85f)[iVar4] = (&DAT_8008f85f)[iVar4] & 1;
      iVar4 = iVar3 * 0x10000;
    } while (iVar3 * 0x10000 >> 0x10 < (int)(uint)DAT_800b52a8);
  }
  (&DAT_8008f85f)[(param_1 & 0xff) * 0x34] = 2;
  uVar2 = (ushort)(1 << (uVar5 - 0x10 & 0x1f));
  DAT_8008ff88 = uVar2 | DAT_8008ff88;
  DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
  DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
  if ((DAT_800b5328 & 4) == 0) {
    SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
    SPU_VOICE_CHN_NOISE_MODE._2_2_ = uVar2;
    DAT_8008ff90 = DAT_8008ff90 & ~uVar2;
    return;
  }
  DAT_8008ff90 = uVar2 | DAT_8008ff90;
  VMANAGER_OBJ_E10();
  return;
}
