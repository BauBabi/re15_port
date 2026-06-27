void VMANAGER_OBJ_41DC(undefined4 param_1,ushort param_2,ushort param_3)

{
  uint in_v1;
  uint uVar1;
  
  uVar1 = in_v1 & 0xffff;
  (&DAT_8008f85f)[uVar1 * 0x34] = 0;
  (&DAT_8008f848)[uVar1 * 0x1a] = 0;
  (&DAT_8008f844)[uVar1 * 0x1a] = 0;
  DAT_800bee78 = param_3 | DAT_800bee78;
  DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
  DAT_800bee80 = param_2 | DAT_800bee80;
  DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
  DAT_800b283c = 0;
  VMANAGER_OBJ_428C();
  return;
}
