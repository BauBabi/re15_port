void _SsSeqCalledTbyT_1per2(void)

{
  if (DAT_800776e8 == 0) {
    DAT_800776e8 = 1;
    SSSTART_OBJ_328();
    return;
  }
  DAT_800776e8 = 0;
  SsSeqCalledTbyT();
  return;
}
