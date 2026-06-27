int VMANAGER_OBJ_3084(undefined4 param_1,int param_2,ushort param_3,uint param_4)

{
  ushort uVar1;
  int iVar2;
  uint in_v1;
  uint uVar3;
  byte in_t0;
  int in_t1;
  int in_t2;
  int in_t3;
  int in_t4;
  int in_t5;
  
  while( true ) {
    uVar3 = in_v1 & 0xffff;
    (&DAT_8008f85f)[uVar3 * 0x34] = 0;
    (&DAT_8008f848)[uVar3 * 0x1a] = 0;
    (&DAT_8008f844)[uVar3 * 0x1a] = 0;
    DAT_800bee78 = param_3 | DAT_800bee78;
    DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
    DAT_800bee80 = (ushort)param_2 | DAT_800bee80;
    DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
    in_t1 = in_t1 + 1;
    do {
      in_t0 = in_t0 + 1;
      uVar3 = (uint)in_t0;
      if (DAT_800b52a8 <= in_t0) {
        return in_t1;
      }
    } while (((((int)(short)(&DAT_8008f850)[uVar3 * 0x1a] != (param_4 & 0xffff)) ||
              ((short)(&DAT_8008f856)[uVar3 * 0x1a] != in_t5)) ||
             ((short)(&DAT_8008f852)[uVar3 * 0x1a] != in_t4)) ||
            ((short)(&DAT_8008f85a)[uVar3 * 0x1a] != in_t3));
    uVar1 = (ushort)in_t0;
    if ((&DAT_8008f844)[uVar3 * 0x1a] == 0xff) break;
    in_v1 = (uint)uVar1;
    DAT_800b532e = uVar1;
    if (in_v1 < 0x10) {
      iVar2 = VMANAGER_OBJ_3084(in_v1,0,in_t2 << (in_v1 & 0x1f));
      return iVar2;
    }
    param_3 = 0;
    param_2 = in_t2 << (in_v1 - 0x10 & 0x1f);
  }
  (&DAT_8008f85f)[uVar3 * 0x34] = 0;
  (&DAT_8008f848)[uVar3 * 0x1a] = 0;
  SPU_VOICE_CHN_NOISE_MODE._0_2_ = 0;
  SPU_VOICE_CHN_NOISE_MODE._2_2_ = 0;
  iVar2 = VMANAGER_OBJ_3118();
  return iVar2;
}
