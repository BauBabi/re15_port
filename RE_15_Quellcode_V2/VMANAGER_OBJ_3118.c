int VMANAGER_OBJ_3118(void)

{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  uint in_a3;
  byte in_t0;
  int in_t1;
  int in_t2;
  int in_t3;
  int in_t4;
  int in_t5;
  
  while( true ) {
    in_t1 = in_t1 + 1;
    do {
      in_t0 = in_t0 + 1;
      uVar3 = (uint)in_t0;
      if (DAT_800b52a8 <= in_t0) {
        return in_t1;
      }
    } while (((((int)(short)(&DAT_8008f850)[uVar3 * 0x1a] != (in_a3 & 0xffff)) ||
              ((short)(&DAT_8008f856)[uVar3 * 0x1a] != in_t5)) ||
             ((short)(&DAT_8008f852)[uVar3 * 0x1a] != in_t4)) ||
            ((short)(&DAT_8008f85a)[uVar3 * 0x1a] != in_t3));
    uVar1 = (ushort)in_t0;
    if ((&DAT_8008f844)[uVar3 * 0x1a] == 0xff) break;
    uVar3 = (uint)uVar1;
    DAT_800b532e = uVar1;
    if (uVar3 < 0x10) {
      iVar2 = VMANAGER_OBJ_3084(uVar3,0,in_t2 << (uVar3 & 0x1f));
      return iVar2;
    }
    (&DAT_8008f85f)[uVar3 * 0x34] = 0;
    (&DAT_8008f848)[uVar3 * 0x1a] = 0;
    (&DAT_8008f844)[uVar3 * 0x1a] = 0;
    DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
    DAT_800bee80 = (ushort)(in_t2 << (uVar3 - 0x10 & 0x1f)) | DAT_800bee80;
    DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
  }
  (&DAT_8008f85f)[uVar3 * 0x34] = 0;
  (&DAT_8008f848)[uVar3 * 0x1a] = 0;
  SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
  SPU_VOICE_CHN_NOISE_MODE._2_2_ = 0;
  iVar2 = VMANAGER_OBJ_3118();
  return iVar2;
}
