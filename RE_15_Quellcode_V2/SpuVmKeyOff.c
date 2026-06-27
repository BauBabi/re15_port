int SpuVmKeyOff(short param_1,short param_2,short param_3,uint param_4)

{
  ushort uVar1;
  uint uVar2;
  byte bVar3;
  int iVar4;
  
  bVar3 = 0;
  iVar4 = 0;
  if (DAT_800b52a8 != 0) {
    uVar2 = 0;
    do {
      if (((((int)(short)(&DAT_8008f850)[uVar2 * 0x1a] == (param_4 & 0xffff)) &&
           ((&DAT_8008f856)[uVar2 * 0x1a] == param_3)) && ((&DAT_8008f852)[uVar2 * 0x1a] == param_1)
          ) && ((&DAT_8008f85a)[uVar2 * 0x1a] == param_2)) {
        uVar1 = (ushort)bVar3;
        if ((&DAT_8008f844)[uVar2 * 0x1a] == 0xff) {
          (&DAT_8008f85f)[uVar2 * 0x34] = 0;
          (&DAT_8008f848)[uVar2 * 0x1a] = 0;
          SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
          SPU_VOICE_CHN_NOISE_MODE._2_2_ = 0;
          iVar4 = VMANAGER_OBJ_3118();
          return iVar4;
        }
        uVar2 = (uint)uVar1;
        DAT_800b532e = uVar1;
        if (uVar2 < 0x10) {
          iVar4 = VMANAGER_OBJ_3084(uVar2,0,1 << (uVar2 & 0x1f));
          return iVar4;
        }
        (&DAT_8008f85f)[uVar2 * 0x34] = 0;
        (&DAT_8008f848)[uVar2 * 0x1a] = 0;
        (&DAT_8008f844)[uVar2 * 0x1a] = 0;
        DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
        DAT_800bee80 = (ushort)(1 << (uVar2 - 0x10 & 0x1f)) | DAT_800bee80;
        DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
        iVar4 = iVar4 + 1;
      }
      bVar3 = bVar3 + 1;
      uVar2 = (uint)bVar3;
    } while (bVar3 < DAT_800b52a8);
  }
  return iVar4;
}
