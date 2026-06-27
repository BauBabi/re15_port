/* SpuSetNoiseVoice @ 0x80081a00  (Ghidra headless, raw MIPS overlay) */

ulong SpuSetNoiseVoice(long on_off,ulong voice_bit)

{
  ulong uVar1;
  
  uVar1 = FUN_80081a24(on_off,voice_bit,0xca,0xcb);
  return uVar1;
}


