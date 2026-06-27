/* SpuSetReverbVoice @ 0x8008235c  (Ghidra headless, raw MIPS overlay) */

ulong SpuSetReverbVoice(long on_off,ulong voice_bit)

{
  ulong uVar1;
  
  uVar1 = FUN_80081a24(on_off,voice_bit,0xcc,0xcd);
  return uVar1;
}


