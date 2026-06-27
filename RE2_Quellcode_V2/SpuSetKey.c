/* SpuSetKey @ 0x80079498  (Ghidra headless, raw MIPS overlay) */

void SpuSetKey(long on_off,ulong voice_bit)

{
  undefined *puVar1;
  uint uVar2;
  ushort uVar3;
  ushort uVar4;
  
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  uVar2 = voice_bit & 0xffffff;
  uVar3 = (ushort)uVar2;
  uVar4 = (ushort)(uVar2 >> 0x10);
  if (on_off == 0) {
    if ((DAT_800ab6e4 & 1) == 0) {
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x18c) = uVar3;
      *(ushort *)(puVar1 + 0x18e) = uVar4;
      DAT_800ab284 = DAT_800ab284 & ~uVar2;
    }
    else {
      DAT_800ab2b0 = DAT_800ab2b0 | 1;
      DAT_800ab2ac = DAT_800ab2ac & ~uVar2;
      if ((DAT_800d5b88 & uVar2) != 0) {
        DAT_800d5b88 = DAT_800d5b88 & (ushort)~uVar2;
      }
      DAT_800d5b8c = uVar3;
      DAT_800d5b8e = uVar4;
      if (((uint)DAT_800d5b8a & uVar2 >> 0x10) != 0) {
        DAT_800d5b8a = DAT_800d5b8a & ~uVar4;
        S_SK_OBJ_1F4();
        return;
      }
    }
  }
  else if (on_off == 1) {
    if ((DAT_800ab6e4 & 1) == 0) {
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x188) = uVar3;
      *(ushort *)(puVar1 + 0x18a) = uVar4;
      S_SK_OBJ_1EC();
      return;
    }
    DAT_800ab2b0 = DAT_800ab2b0 | 1;
    DAT_800ab2ac = DAT_800ab2ac | uVar2;
    if ((DAT_800d5b8c & uVar2) != 0) {
      DAT_800d5b8c = DAT_800d5b8c & ~uVar3;
    }
    DAT_800d5b88 = uVar3;
    DAT_800d5b8a = uVar4;
    if (((uint)DAT_800d5b8e & uVar2 >> 0x10) != 0) {
      DAT_800d5b8e = DAT_800d5b8e & ~uVar4;
      S_SK_OBJ_1F4();
      return;
    }
  }
  return;
}


