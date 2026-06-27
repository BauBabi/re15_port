DISPENV * GetDispEnv(DISPENV *env)

{
  memcpy((uchar *)env,"",0x14);
  return env;
}
