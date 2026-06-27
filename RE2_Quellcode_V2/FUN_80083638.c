/* FUN_80083638 @ 0x80083638  (Ghidra headless, raw MIPS overlay) */

void FUN_80083638(uint param_1)

{
  (&DAT_800cbcd4)[(param_1 & 0xff) * 0x1b] = 0;
  (&DAT_800cbced)[(param_1 & 0xff) * 0x36] = 0;
  SpuSetNoiseVoice(0,0xffffff);
  return;
}


