void VMANAGER_OBJ_670(undefined4 param_1,ushort param_2,ushort param_3)

{
  if ((DAT_800b5328 & 4) != 0) {
    DAT_8008ff8c = param_3 | DAT_8008ff8c;
    DAT_8008ff90 = param_2 | DAT_8008ff90;
    VMANAGER_OBJ_6E8();
    return;
  }
  DAT_8008ff8c = DAT_8008ff8c & ~param_3;
  DAT_8008ff90 = DAT_8008ff90 & ~param_2;
  DAT_8008ff84 = param_3 | DAT_8008ff84;
  DAT_800bee78 = DAT_800bee78 & ~DAT_8008ff84;
  DAT_8008ff88 = param_2 | DAT_8008ff88;
  DAT_800bee80 = DAT_800bee80 & ~DAT_8008ff88;
  return;
}
