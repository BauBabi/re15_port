short SsUtKeyOffV(short voice)

{
  short sVar1;
  uint uVar2;
  
  if (DAT_800b283c != 1) {
    DAT_800b283c = 1;
    if ((ushort)voice < 0x18) {
      uVar2 = (uint)(ushort)voice;
      DAT_800b532e = voice;
      if (uVar2 < 0x10) {
        sVar1 = VMANAGER_OBJ_4690(uVar2,1,0,1 << (uVar2 & 0x1f));
        return sVar1;
      }
      (&DAT_8008f85f)[uVar2 * 0x34] = 0;
      (&DAT_8008f848)[uVar2 * 0x1a] = 0;
      (&DAT_8008f844)[uVar2 * 0x1a] = 0;
      DAT_800b283c = 0;
      DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
      DAT_800bee80 = (ushort)(1 << (uVar2 - 0x10 & 0x1f)) | DAT_800bee80;
      DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
      sVar1 = VMANAGER_OBJ_4740();
      return sVar1;
    }
    DAT_800b283c = 0;
  }
  return -1;
}
