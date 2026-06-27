/* SsSepReplay @ 0x8007cd28  (Ghidra headless, raw MIPS overlay) */

/* Possible PLAY.OBJ/_SsSndPlay */

void SsSepReplay(short param_1,short param_2)

{
  _SsSndSetReplayMode((int)param_1,(int)param_2);
  return;
}


