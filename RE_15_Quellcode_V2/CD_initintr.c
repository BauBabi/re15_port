void CD_initintr(void)

{
  DAT_800788a8 = 0;
  DAT_800788a4 = 0;
  DAT_800788b8 = 0;
  _DAT_800788b4 = 0;
  ResetCallback();
  InterruptCallback(2,callback);
  return;
}
