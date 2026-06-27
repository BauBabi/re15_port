uint VMANAGER_OBJ_144(void)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  byte in_a3;
  byte bVar5;
  int in_t0;
  char in_t2;
  ushort in_t3;
  uint in_t4;
  byte unaff_s0;
  
  bVar1 = DAT_800b52a8;
  bVar5 = in_a3;
  while( true ) {
    in_a3 = in_a3 + 1;
    if (DAT_800b52a8 <= in_a3) {
      if ((unaff_s0 == 99) && (unaff_s0 = bVar5, in_t2 == '\0')) {
        unaff_s0 = DAT_800b52a8;
      }
      if (unaff_s0 < DAT_800b52a8) {
        bVar5 = 0;
        if (DAT_800b52a8 != 0) {
          uVar2 = 0;
          do {
            bVar5 = bVar5 + 1;
            (&DAT_8008f846)[uVar2 * 0x1a] = (&DAT_8008f846)[uVar2 * 0x1a] + 1;
            uVar2 = (uint)bVar5;
          } while (bVar5 < bVar1);
        }
        uVar2 = (uint)unaff_s0;
        (&DAT_8008f846)[uVar2 * 0x1a] = 0;
        (&DAT_8008f85c)[uVar2 * 0x1a] = (ushort)DAT_800b5323;
        if ((&DAT_8008f85f)[uVar2 * 0x34] == '\x02') {
          SpuSetNoiseVoice(0,0xffffff);
        }
      }
      return (uint)unaff_s0;
    }
    if (((&DAT_8008f85f)[(uint)in_a3 * 0x34] == '\0') && ((&DAT_8008f84a)[(uint)in_a3 * 0x1a] == 0))
    break;
    uVar2 = (uint)in_a3;
    uVar4 = (uint)(short)(&DAT_8008f85c)[uVar2 * 0x1a];
    uVar3 = in_t4 & 0xffff;
    if ((int)uVar4 < (int)uVar3) {
      uVar2 = VMANAGER_OBJ_148(uVar3,uVar4);
      return uVar2;
    }
    if (uVar4 == uVar3) {
      in_t2 = in_t2 + '\x01';
      if ((ushort)(&DAT_8008f84a)[uVar2 * 0x1a] < in_t3) {
        uVar2 = VMANAGER_OBJ_144();
        return uVar2;
      }
      if (((&DAT_8008f84a)[uVar2 * 0x1a] == in_t3) && (in_t0 < (short)(&DAT_8008f846)[uVar2 * 0x1a])
         ) {
        in_t0 = (int)(short)(&DAT_8008f846)[uVar2 * 0x1a];
        bVar5 = in_a3;
      }
    }
  }
  uVar2 = VMANAGER_OBJ_164();
  return uVar2;
}
