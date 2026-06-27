short SsUtKeyOff(short param_1,short param_2,short param_3,short param_4,short param_5)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  
  if (DAT_800b283c != 1) {
    DAT_800b283c = 1;
    if (((((ushort)param_1 < 0x18) &&
         (iVar2 = (int)param_1, (&DAT_8008f85a)[iVar2 * 0x1a] == param_2)) &&
        ((&DAT_8008f856)[iVar2 * 0x1a] == param_3)) &&
       (((&DAT_8008f858)[iVar2 * 0x1a] == param_4 && ((&DAT_8008f850)[iVar2 * 0x1a] == param_5)))) {
      if ((&DAT_8008f844)[iVar2 * 0x1a] == 0xff) {
        (&DAT_8008f85f)[((ushort)param_1 & 0xff) * 0x34] = 0;
        (&DAT_8008f848)[((ushort)param_1 & 0xff) * 0x1a] = 0;
        SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
        SPU_VOICE_CHN_NOISE_MODE._2_2_ = 0;
        sVar1 = VMANAGER_OBJ_4270();
        return sVar1;
      }
      uVar3 = (uint)(ushort)param_1;
      DAT_800b532e = param_1;
      if (uVar3 < 0x10) {
        sVar1 = VMANAGER_OBJ_41DC(uVar3,0,1 << (uVar3 & 0x1f));
        return sVar1;
      }
      (&DAT_8008f85f)[uVar3 * 0x34] = 0;
      (&DAT_8008f848)[uVar3 * 0x1a] = 0;
      (&DAT_8008f844)[uVar3 * 0x1a] = 0;
      DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
      DAT_800bee80 = (ushort)(1 << (uVar3 - 0x10 & 0x1f)) | DAT_800bee80;
      DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
      DAT_800b283c = 0;
      sVar1 = VMANAGER_OBJ_428C();
      return sVar1;
    }
    DAT_800b283c = 0;
  }
  return -1;
}
