undefined4 StopRCnt(uint param_1)

{
  _I_MASK = ~*(uint *)(&DAT_8008f5bc + (param_1 & 0xffff) * 4) & _I_MASK;
  return 1;
}
