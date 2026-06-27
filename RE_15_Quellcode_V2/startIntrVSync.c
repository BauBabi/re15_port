code * startIntrVSync(void)

{
  TMR_HRETRACE_MODE = 0x107;
  DAT_800787dc = 0;
  memclr(&DAT_800787bc,8);
  InterruptCallback(0,trapIntrVSync);
  return setIntrVSync;
}
