byte VMANAGER_OBJ_164(void)

{
  byte bVar1;
  uint uVar2;
  byte bVar3;
  byte in_t1;
  char in_t2;
  byte unaff_s0;
  
  bVar1 = DAT_800b52a8;
  if ((unaff_s0 == 99) && (unaff_s0 = in_t1, in_t2 == '\0')) {
    unaff_s0 = DAT_800b52a8;
  }
  if (unaff_s0 < DAT_800b52a8) {
    bVar3 = 0;
    if (DAT_800b52a8 != 0) {
      uVar2 = 0;
      do {
        bVar3 = bVar3 + 1;
        (&DAT_8008f846)[uVar2 * 0x1a] = (&DAT_8008f846)[uVar2 * 0x1a] + 1;
        uVar2 = (uint)bVar3;
      } while (bVar3 < bVar1);
    }
    uVar2 = (uint)unaff_s0;
    (&DAT_8008f846)[uVar2 * 0x1a] = 0;
    (&DAT_8008f85c)[uVar2 * 0x1a] = (ushort)DAT_800b5323;
    if ((&DAT_8008f85f)[uVar2 * 0x34] == '\x02') {
      SpuSetNoiseVoice(0,0xffffff);
    }
  }
  return unaff_s0;
}
