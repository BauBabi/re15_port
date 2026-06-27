void VMANAGER_OBJ_1FC4(undefined4 param_1,uint param_2,int param_3,ushort param_4)

{
  uint uVar1;
  uint in_v1;
  uint uVar2;
  int in_t0;
  undefined2 in_t1;
  
  while( true ) {
    uVar2 = in_v1 & 0xffff;
    (&DAT_8008f85f)[uVar2 * 0x34] = 0;
    param_2 = param_2 + 1;
    (&DAT_8008f848)[uVar2 * 0x1a] = 0;
    (&DAT_8008f844)[uVar2 * 0x1a] = 0;
    DAT_800bee78 = param_4 | DAT_800bee78;
    DAT_800bee80 = (ushort)param_3 | DAT_800bee80;
    DAT_8008ff84 = DAT_8008ff84 & ~DAT_800bee78;
    DAT_8008ff88 = DAT_8008ff88 & ~DAT_800bee80;
    uVar2 = param_2 & 0xffff;
    if ((uint)DAT_800b52a8 <= (param_2 & 0xffff)) {
      DAT_800b21fc = 0x3fff;
      DAT_800b21fe = 0x3fff;
      DAT_8008ff84 = 0;
      DAT_8008ff88 = 0;
      DAT_800bee78 = 0;
      DAT_8008ff8c = 0;
      DAT_8008ff90 = 0;
      DAT_800b21f4 = 0;
      _DAT_800b21f8 = 0;
      DAT_800b535c = 0;
      DAT_800b2678 = 0;
      DAT_800b284c = 0x80;
      SpuVmFlush();
      return;
    }
    (&DAT_8008f846)[uVar2 * 0x1a] = 0x18;
    (&DAT_8008f852)[uVar2 * 0x1a] = 0xffff;
    uVar1 = param_2 & 0x1fff;
    (&DAT_8008f844)[uVar2 * 0x1a] = in_t1;
    (&DAT_8008f85f)[uVar2 * 0x34] = 0;
    (&DAT_8008f848)[uVar2 * 0x1a] = 0;
    (&DAT_8008f84a)[uVar2 * 0x1a] = 0;
    (&DAT_8008f854)[uVar2 * 0x1a] = 0;
    (&DAT_8008f856)[uVar2 * 0x1a] = 0;
    (&DAT_8008f858)[uVar2 * 0x1a] = in_t1;
    (&DAT_8008f84c)[uVar2 * 0x1a] = 0;
    (&DAT_8008f84e)[uVar2 * 0x34] = 0x40;
    (&DAT_8008f860)[uVar2 * 0x1a] = 0;
    (&DAT_8008f862)[uVar2 * 0x1a] = 0;
    (&DAT_8008f864)[uVar2 * 0x1a] = 0;
    (&DAT_8008f866)[uVar2 * 0x1a] = 0;
    (&DAT_8008f86c)[uVar2 * 0x1a] = 0;
    (&DAT_8008f86e)[uVar2 * 0x1a] = 0;
    (&DAT_8008f870)[uVar2 * 0x1a] = 0;
    (&DAT_8008f872)[uVar2 * 0x1a] = 0;
    (&DAT_8008f874)[uVar2 * 0x1a] = 0;
    (&DAT_8008f868)[uVar2 * 0x1a] = 0;
    (&VOICE_00_ADPCM_START_ADDR)[uVar1 * 8] = 0x200;
    (&VOICE_00_ADPCM_SAMPLE_RATE)[uVar1 * 8] = 0x1000;
    (&VOICE_00_ADSR_ATT_DEC_SUS_REL)[uVar1 * 8] = 0x80ff;
    *(undefined2 *)(&VOICE_00_LEFT_RIGHT + uVar1 * 4) = 0;
    *(undefined2 *)((int)&VOICE_00_LEFT_RIGHT + uVar1 * 0x10 + 2) = 0;
    (&DAT_1f801c0a)[uVar1 * 8] = 0x4000;
    DAT_800b532e = (undefined2)param_2;
    in_v1 = param_2 & 0xffff;
    if (in_v1 < 0x10) break;
    param_4 = 0;
    param_3 = in_t0 << (in_v1 - 0x10 & 0x1f);
  }
  VMANAGER_OBJ_1FC4(in_v1,param_2,0,in_t0 << (param_2 & 0x1f));
  return;
}
