void SsUtAllKeyOff(short param_1)

{
  uint uVar1;
  int iVar2;
  ushort uVar3;
  
  if (DAT_800b52a8 != 0) {
    iVar2 = 0;
    uVar3 = 0;
    do {
      iVar2 = iVar2 >> 0x10;
      (&DAT_8008f846)[iVar2 * 0x1a] = 0x18;
      (&DAT_8008f844)[iVar2 * 0x1a] = 0xff;
      (&DAT_8008f85f)[iVar2 * 0x34] = 0;
      (&DAT_8008f848)[iVar2 * 0x1a] = 0;
      (&DAT_8008f84a)[iVar2 * 0x1a] = 0;
      (&DAT_8008f852)[iVar2 * 0x1a] = 0xff;
      (&DAT_8008f854)[iVar2 * 0x1a] = 0;
      (&DAT_8008f856)[iVar2 * 0x1a] = 0;
      (&DAT_8008f858)[iVar2 * 0x1a] = 0xff;
      iVar2 = (iVar2 << 0x13) >> 0xf;
      *(undefined2 *)((int)&VOICE_00_ADPCM_START_ADDR + iVar2) = 0x200;
      *(undefined2 *)((int)&VOICE_00_ADPCM_SAMPLE_RATE + iVar2) = 0x1000;
      *(undefined2 *)((int)&VOICE_00_ADSR_ATT_DEC_SUS_REL + iVar2) = 0x80ff;
      *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + iVar2) = 0;
      *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + iVar2 + 2) = 0;
      *(undefined2 *)((int)&DAT_1f801c0a + iVar2) = 0x4000;
      uVar1 = (uint)uVar3;
      DAT_800b532e = uVar3;
      if (uVar1 < 0x10) {
        VMANAGER_OBJ_4D90();
        return;
      }
      uVar3 = uVar3 + 1;
      (&DAT_8008f85f)[uVar1 * 0x34] = 0;
      (&DAT_8008f848)[uVar1 * 0x1a] = 0;
      (&DAT_8008f844)[uVar1 * 0x1a] = 0;
      DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
      DAT_800bee80 = (ushort)(1 << (uVar1 - 0x10 & 0x1f)) | DAT_800bee80;
      DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
      iVar2 = (uint)uVar3 << 0x10;
    } while ((short)uVar3 < (short)(ushort)DAT_800b52a8);
  }
  return;
}
