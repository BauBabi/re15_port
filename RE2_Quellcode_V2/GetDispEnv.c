/* GetDispEnv @ 0x80091114  (Ghidra headless, raw MIPS overlay) */

DISPENV * GetDispEnv(DISPENV *env)

{
  memcpy((uchar *)env,"",0x14);
  return env;
}


