ulong SpuSetNoiseVoice(long on_off,ulong voice_bit)

{
  ulong uVar1;
  
  uVar1 = _SpuSetAnyVoice(on_off,voice_bit,0xca,0xcb);
  return uVar1;
}
