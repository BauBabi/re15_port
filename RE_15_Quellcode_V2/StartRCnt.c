bool StartRCnt(uint param_1)

{
  _I_MASK = _I_MASK | *(uint *)(&DAT_8008f5bc + (param_1 & 0xffff) * 4);
  return (param_1 & 0xffff) < 3;
}
