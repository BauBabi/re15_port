/* startIntrVSync @ 0x80086794  (Ghidra headless, raw MIPS overlay) */

code * startIntrVSync(void)

{
  *(undefined4 *)PTR_TMR_HRETRACE_MODE_800acd04 = 0x107;
  DAT_800acd00 = 0;
  memclr(&DAT_800acce0,8);
  InterruptCallback(0,trapIntrVSync);
  return setIntrVSync;
}


