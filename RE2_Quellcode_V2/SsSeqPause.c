/* SsSeqPause @ 0x8007cb04  (Ghidra headless, raw MIPS overlay) */

void SsSeqPause(short param_1)

{
  _SsSndSetPauseMode((int)param_1,0);
  return;
}


