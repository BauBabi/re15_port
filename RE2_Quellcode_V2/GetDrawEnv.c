/* GetDrawEnv @ 0x80090c34  (Ghidra headless, raw MIPS overlay) */

DRAWENV * GetDrawEnv(DRAWENV *env)

{
  memcpy((uchar *)env,(uchar *)&DAT_800b2710,0x5c);
  return env;
}


