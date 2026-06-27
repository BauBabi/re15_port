DRAWENV * GetDrawEnv(DRAWENV *env)

{
  memcpy((uchar *)env,"",0x5c);
  return env;
}
