/* _SsSndPlay @ 0x8007cb2c  (Ghidra headless, raw MIPS overlay) */

/* Possible SSPAUSE.OBJ/SsSepPause */

void _SsSndPlay(short param_1,short param_2)

{
  _SsSndSetPauseMode((int)param_1,(int)param_2);
  return;
}


