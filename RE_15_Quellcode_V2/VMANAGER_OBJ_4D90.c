void VMANAGER_OBJ_4D90(undefined4 param_1,undefined4 param_2,int param_3,ushort param_4)

{
  bool bVar1;
  uint in_v1;
  uint uVar2;
  short sVar3;
  int iVar4;
  uint in_t0;
  undefined2 in_t1;
  int in_t2;
  
  while( true ) {
    in_t0 = in_t0 + 1;
    uVar2 = in_v1 & 0xffff;
    (&DAT_8008f85f)[uVar2 * 0x34] = 0;
    (&DAT_8008f848)[uVar2 * 0x1a] = 0;
    (&DAT_8008f844)[uVar2 * 0x1a] = 0;
    DAT_800bee78 = param_4 | DAT_800bee78;
    DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
    DAT_800bee80 = (ushort)param_3 | DAT_800bee80;
    DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
    bVar1 = (int)(in_t0 * 0x10000) >> 0x10 < (int)(uint)DAT_800b52a8;
    sVar3 = (short)in_t0;
    if (!bVar1) {
      return;
    }
    iVar4 = (int)sVar3;
    (&DAT_8008f846)[iVar4 * 0x1a] = 0x18;
    (&DAT_8008f844)[iVar4 * 0x1a] = in_t1;
    (&DAT_8008f85f)[iVar4 * 0x34] = 0;
    (&DAT_8008f848)[iVar4 * 0x1a] = 0;
    (&DAT_8008f84a)[iVar4 * 0x1a] = 0;
    (&DAT_8008f852)[iVar4 * 0x1a] = in_t1;
    (&DAT_8008f854)[iVar4 * 0x1a] = 0;
    (&DAT_8008f856)[iVar4 * 0x1a] = 0;
    (&DAT_8008f858)[iVar4 * 0x1a] = in_t1;
    iVar4 = (iVar4 << 0x13) >> 0xf;
    *(undefined2 *)((int)&VOICE_00_ADPCM_START_ADDR + iVar4) = 0x200;
    *(undefined2 *)((int)&VOICE_00_ADPCM_SAMPLE_RATE + iVar4) = 0x1000;
    *(undefined2 *)((int)&VOICE_00_ADSR_ATT_DEC_SUS_REL + iVar4) = 0x80ff;
    *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + iVar4) = 0;
    *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + iVar4 + 2) = 0;
    *(undefined2 *)((int)&DAT_1f801c0a + iVar4) = 0x4000;
    in_v1 = in_t0 & 0xffff;
    DAT_800b532e = sVar3;
    if (in_v1 < 0x10) break;
    param_4 = 0;
    param_3 = in_t2 << (in_v1 - 0x10 & 0x1f);
  }
  VMANAGER_OBJ_4D90(in_v1,bVar1,0,in_t2 << (in_t0 & 0x1f));
  return;
}
