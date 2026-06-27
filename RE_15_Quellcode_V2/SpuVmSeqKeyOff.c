void SpuVmSeqKeyOff(short param_1)

{
  ushort uVar1;
  uint uVar2;
  byte bVar3;
  
  bVar3 = 0;
  if (DAT_800b52a8 != 0) {
    uVar2 = 0;
    do {
      uVar1 = (ushort)bVar3;
      if ((&DAT_8008f852)[uVar2 * 0x1a] == param_1) {
        uVar2 = (uint)uVar1;
        DAT_800b532e = uVar1;
        if (uVar2 < 0x10) {
          VMANAGER_OBJ_357C(uVar2,0,1 << (uVar2 & 0x1f));
          return;
        }
        (&DAT_8008f85f)[uVar2 * 0x34] = 0;
        (&DAT_8008f848)[uVar2 * 0x1a] = 0;
        (&DAT_8008f844)[uVar2 * 0x1a] = 0;
        DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
        DAT_800bee80 = (ushort)(1 << (uVar2 - 0x10 & 0x1f)) | DAT_800bee80;
        DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
      }
      bVar3 = bVar3 + 1;
      uVar2 = (uint)bVar3;
    } while (bVar3 < DAT_800b52a8);
  }
  return;
}
