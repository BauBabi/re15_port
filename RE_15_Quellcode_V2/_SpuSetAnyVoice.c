uint _SpuSetAnyVoice(int param_1,uint param_2,int param_3,int param_4)

{
  ushort *puVar1;
  ushort *puVar2;
  uint uVar3;
  
  puVar2 = (ushort *)((int)&VOICE_00_LEFT_RIGHT + param_4 * 2);
  puVar1 = (ushort *)((int)&VOICE_00_LEFT_RIGHT + param_3 * 2);
  uVar3 = (uint)*puVar1 | (*puVar2 & 0xff) << 0x10;
  if (param_1 == 0) {
    uVar3 = uVar3 & ~(param_2 & 0xffffff);
    *puVar1 = *puVar1 & ~(ushort)param_2;
    *puVar2 = *puVar2 & ~((ushort)(param_2 >> 0x10) & 0xff);
  }
  else if (param_1 == 1) {
    *puVar1 = *puVar1 | (ushort)param_2;
    uVar3 = S_SAV_OBJ_A0();
    return uVar3;
  }
  return uVar3;
}
