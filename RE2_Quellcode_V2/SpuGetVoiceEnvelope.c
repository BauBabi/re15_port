/* SpuGetVoiceEnvelope @ 0x80082380  (Ghidra headless, raw MIPS overlay) */

void SpuGetVoiceEnvelope(int vNum,short *envx)

{
  *envx = *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + vNum * 0x10 + 0xc);
  return;
}


