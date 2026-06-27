void VMANAGER_OBJ_4690(uint param_1,undefined4 param_2,ushort param_3,ushort param_4)

{
  param_1 = param_1 & 0xffff;
  (&DAT_8008f85f)[param_1 * 0x34] = 0;
  (&DAT_8008f848)[param_1 * 0x1a] = 0;
  (&DAT_8008f844)[param_1 * 0x1a] = 0;
  DAT_800b283c = 0;
  DAT_800bee78 = param_4 | DAT_800bee78;
  DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
  DAT_800bee80 = param_3 | DAT_800bee80;
  DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
  VMANAGER_OBJ_4740();
  return;
}
