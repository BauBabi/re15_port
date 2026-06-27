void VMANAGER_OBJ_357C(undefined4 param_1,int param_2,ushort param_3,byte param_4)

{
  uint in_v1;
  uint uVar1;
  int in_t0;
  int in_t1;
  ushort *in_t2;
  
  while( true ) {
    uVar1 = in_v1 & 0xffff;
    (&DAT_8008f85f)[uVar1 * 0x34] = 0;
    (&DAT_8008f848)[uVar1 * 0x1a] = 0;
    (&DAT_8008f844)[uVar1 * 0x1a] = 0;
    DAT_800bee78 = param_3 | DAT_800bee78;
    DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
    DAT_800bee80 = (ushort)param_2 | DAT_800bee80;
    DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
    do {
      param_4 = param_4 + 1;
      if (DAT_800b52a8 <= param_4) {
        return;
      }
    } while ((short)(&DAT_8008f852)[(uint)param_4 * 0x1a] != in_t1);
    *in_t2 = (ushort)param_4;
    in_v1 = (uint)DAT_800b532e;
    if (in_v1 < 0x10) break;
    param_3 = 0;
    param_2 = in_t0 << (in_v1 - 0x10 & 0x1f);
  }
  VMANAGER_OBJ_357C(in_v1,0,in_t0 << (in_v1 & 0x1f));
  return;
}
